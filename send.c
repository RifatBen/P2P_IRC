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
/*
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
*/
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
	

int sntp_req(int s,struct sockaddr_in6 *peer, TLV tlv){
	int rc=sendto(s,&tlv,sizeof(tlv),0,(struct sockaddr*)peer,sizeof(struct sockaddr_in)) ;
	if(rc<0)
		printf("%s","Erreur de l'envoie");
}


void Envoi_LOng(TLV tlv,int s){
	tlv.length=16;
	
		while(p->recent->first!=NULL){
			sntp_req(s,&p,tlv);
			p=p->recent->first->next;
		}
	
}

void Envoi_Court(TLV tlv,int s){
	tlv.length=8;
	
	while(p->potentiel->first!=NULL){
			sntp_req(s,&p,tlv);
			p=p->potentiel->first->next;
		}
	
}


int recv_req(int s,struct sockaddr_in6 *peer, TLV tlv){
	int rc=recvfrom(s,&tlv,sizeof(tlv),0,(struct sockaddr*)&peer,sizeof(struct sockaddr_in)) ;
	if(rc<0){
		perror("recvfrom");
		exit(1);
	}
}

void Supprime(Peer *peer){
if(p!=NULL){
	while(p->potentiel->first->id!=peer->id){
		p=p->potentiel->first->next;
	}
	if(p->potentiel->first->next!=NULL){
		p->potentiel->first->prev->next=p->potentiel->first->next;
		p->potentiel->first->next->prev=p->potentiel->first->prev;
	}else{
		p->potentiel->last=p->potentiel->first->prev;
		p->potentiel->first->prev->next=NULL;
	}
}
return p;
}
		
	
	
	
void ChekHelloCourt(TLV tlv,struct sockaddr_in6 *peer,int s){
	Voisin *v;
	tlv.length=8;
		if(recv_req(s,&peer,tlv)){
			if(isVoisin(p->potentiel,peer->sin6_addr.s6_addr, peer->sin6_port)){
			Supprime(&p);
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
			Supprime(&p);
			v=newVoisin(byteToNumber(tlv.body.Hello.sourceid,64), peer->sin6_addr.s6_addr, peer->sin6_port);
					addVoisin(p->recent,v);
		}
	}
}


