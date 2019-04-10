#include "struct.h"



Voisins *newVoisin(uint8_t id, uint8_t port){
	Voisins *v=malloc(sizeof(Voisin));
	v->id=id;
	v->port=port;
	v->symetrique=0;
	v->*next=NULL;
return v;
}

void addVoisns(Voisins *list,new Voisins){
