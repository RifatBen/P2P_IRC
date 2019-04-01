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


//Des futures réactions lors des récéptions de TLVs
void checkRecieved (TLV tlv){
	switch(tlv.type){

		//Hello
		case 2 :
		//Si court 
			//On ajoute aux voisins récents.
		//Si long
			//Si destId = notre id, on ajoute aux voisins symmétriques
			//Sinon on garde dans les voisins récents
		//Si le voisin est déjà dans la liste des voisins récents on ne fait rien
		break;

		//Neighbor
		case 3 :
		break;

		//data
		case 4 :
		//Traiter le Data
		//Envoyer un ack a l'envoyeur
		break;

		//ack
		case 5 : 
		break;

		//Goaway
		case 6 : 
		//Réagir en fonction du code reçu
		//Si code == 1 : On retire l'emetteur des voisins
		//Si code == 2 : Envoyer un hello long
		//Si code == 3 : Envoyer un hello long

		//Retirer le recepteur des voisins symmétriques/ou retirer de la liste tout court
		//LE garder dans les potentiels voisins
		break;

		//Warning
		case 7 : 
		//Montrer le message warning
		break;

		//0, PadN ou autres
		default : 
		//a Ignorer
		break;
	}
}

