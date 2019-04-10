#include "struct.h"



Voisins *newVoisin(uint8_t id, uint8_t port){
	Voisins *v=malloc(sizeof(Voisins));
	v->id=id;
	v->port=port;
	v->symetrique=0;
	v->*next=NULL;
return v;
}

void addVoisin(Voisins *list, Voisins *newVoisin){
	list->last->next = newVoisin;
	list->last = newVoisin;
}
	
Peer *initPeer(Peer p){
	p->id=NULL;
	p->*potentiel=NULL//ca prend une liste de voisin
	p->*recent=NULL
return p;
}

