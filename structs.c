#include "structs.h"


TLV *newTLV(int type, int length, char *body){
	TLV *message = calloc(1,sizeof(TLV));
	message->type = type;
	message->length = length;
	strncpy(message->body,body,4078);

	return message;
}

Voisins *newVoisin(int id, int port){
	Voisins *voisin = calloc(1,sizeof(Voisins));
	snprintf(voisin->id,8,"%d",id);
	snprintf(voisin->port,5,"%d",port);
	

	
	voisin->next=NULL;

	return voisin;
}