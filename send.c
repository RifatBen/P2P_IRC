#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define REQ_SIZE 14



typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;

typedef struct TLV {
	unsigned char type;
	unsigned char length;
		union{	
			struct{
				unsigned char mbz[4078];
			}PadN;
			
			struct{
				unsigned char sourceid[8];
				unsigned char destinationid[8];
			}Hello;

			struct{
				unsigned char ip[16];
				unsigned char port[2];
			}Neighbour;

			struct{
				unsigned char senderid[8];
				unsigned char nonce[4];
				short type;
				char data[4065];
			}Data;

			struct{
				unsigned char senderid[8];
				unsigned char nonce[4];
			}Ack;

			struct{
				short code;
				char message[4077];
			}GoAway;

			struct{
				char message[4078];
			}Warning;

		}body;
}TLV;

typedef struct Header {
	unsigned char magic;
	unsigned char version;
	unsigned char bodylen[2];
}Header;

typedef struct Datagramme{
	struct header *head;
	struct TLV *body;
}Datagramme;




typedef struct Voisin{
	unsigned char ip[16];
	uint64_t id;
	uint16_t port;
	short symetrique;

	time_t shorthello;
	time_t longhello;
	struct Voisin *prev;//ajout
	struct Voisin *next;
}Voisin;
typedef struct Liste_Voisin{
	Voisin *first;
	Voisin *last;
}Liste_Voisin;
typedef struct Peer{
	uint64_t id;
	Liste_Voisin *potentiel;
	Liste_Voisin *recent;
}Peer;

void addVoisin(Liste_Voisin *list, Voisin *newVoisin){
		if(list->first==NULL){
			list->first = newVoisin;
			list->last = newVoisin;	
		}
		else{
			list->last->next = newVoisin;
			list->last = newVoisin;
		}

}


//Convertie un nombre dans un unsigned char (format big endian) en un nombre (uint64)
uint64_t byteToNumber(unsigned char *req, int size){//
	uint64_t num = 0;
	int j=0;
	for(int i=size-8;i>=0;i-=8){
		num += ((uint64_t)req[j] << i);
		j++;
	}

	return num;

}



//
int isVoisin(Liste_Voisin *list, unsigned char *ip, uint64_t port){
	uint128_t ipNum = byteToNumber(ip,128);

	Voisin *current = list->first;

	while(current!=NULL){
		if(ipNum == byteToNumber(current->ip,128)){
			return 1;
		}
		current = current->next;
	}
	return 0;
}



Voisin *newVoisin(uint64_t id,unsigned char *ip, uint16_t port){
	
	Voisin *v=(Voisin*)malloc(sizeof(Voisin));
	v->id=id;
	memcpy(v->ip,ip,16);
	v->port=port;
	v->symetrique=0;
	v->next=NULL;
	return v;
}



int sntp_req(int s,struct sockaddr_in6 *peer, TLV tlv){
	unsigned char req[REQ_SIZE] = {0};
	createRequest(req,tlv);
	int lenreq = sizeof(req)/sizeof(unsigned char);
	int rc=sendto(s,req,lenreq,0,(struct sockaddr*)peer,sizeof(struct sockaddr_in)) ;
	if(rc < 0) {
      		fprintf(stderr,"Error recev\n");
      		exit(1);
    	}else{
		return 1;
	}
}


void Envoi_LOng(int s,Liste_Voisin *l){
	TLV tlv;
	Voisin *v;
	v=l->first;
	while(v!=NULL){//envoi de Hello long different à chaque voisin
	newHello_Long(&tlv,1024);
	sntp_req(s,&v,tlv);
	v=v->next;
	}
	
}

void Envoi_Court(int s,Liste_Voisin *l){
	TLV tlv;
	newHello_Court(&tlv,1024);
	Voisin *v;
	v=l->first;
	while(v!=NULL){//ENvoi d'un meme Hello Court à chaque voisin
	sntp_req(s,&v,tlv);
	v=v->next;
	}
	
}



void recv_req(int s,struct sockaddr_in6 *peer){
	unsigned char buf [BUF_SIZE] = {0};
	 int rc = recvfrom(s, buf, BUF_SIZE,0,&peer, sizeof(struct sockaddr_in6));
    	if(rc < 0) {
      		fprintf(stderr,"Error recev\n");
      		exit(1);
    	}else{
	 printf("\nMessage reçu\n");
   	 for (int i = 0 ; i < rc ; i++) { 
		printf("%.2d ", buf[i]);
	}
	}
}



void Supprime(Liste_Voisin *l,unsigned char* ip2){
	Voisin *v;
	v=l->first;
	while(v->ip!=ip2 && v->next!=NULL){ 
		v=v->next;
	}
	if(v->next!=NULL){
		v->prev->next=v->next;
		v->next->prev=v->prev;
	}else{
		v->prev=l->last;
		l->last=v->prev;
	}
return l;
}
		
	
	



/*	
void ChekHelloCourt(TLV tlv,struct sockaddr_in6 *peer,int s){
	Voisin *v;
	tlv.length=8;
		if(recv_req(s,&peer,tlv)){
			if(isVoisin(p->potentiel,peer->sin6_addr.s6_addr, peer->sin6_port)){
			Supprime(&p,p->port);
			v=newVoisin(byteToNumber(tlv.body.Hello.sourceid,64), peer->sin6_addr.s6_addr, peer->sin6_port);
					addVoisin(p->recent,v);
			}
		}
}
	
void ChekHelloLOng(TLV tlv,struct sockaddr_in6 *peer,int s){
	Voisin *v;
	tlv.length=16;
	if(recv_req(s,&peer,tlv)){
		if(isVoisin(p->potentiel,peer->sin6_addr.s6_addr, peer->sin6_port)){
			Supprime(&p,p->port);
			v=newVoisin(byteToNumber(tlv.body.Hello.sourceid,64), peer->sin6_addr.s6_addr, peer->sin6_port);
					addVoisin(p->recent,v);
		}
	}
}
*/

