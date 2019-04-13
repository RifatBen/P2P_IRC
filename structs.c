#include "structs.h"
#include "requestHandler.h"

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
	
void initPeer(Peer *p){
	
	p->id=454554; // c censé être random
	p->potentiel=(Liste_Voisin*)malloc(sizeof(Liste_Voisin)); //ca prend une liste de voisin
	p->potentiel->first = NULL;
	p->potentiel->last = NULL;
	p->recent=(Liste_Voisin*) malloc(sizeof(Liste_Voisin));
	p->recent->first = NULL;
	p->recent->last = NULL;
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


void afficheListe(Liste_Voisin *list){
	Voisin *current = list->first;
	char str[30];
	while(current!=NULL){
		inet_ntop(AF_INET6, current->ip, str, INET6_ADDRSTRLEN);
		printf("\n(IP : PORT) : (%s,%d) \n",str,current->port);
		current = current->next;
	}

}