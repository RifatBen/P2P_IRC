#include "requestHandler.h"








//Vérifie si une requête est bien formée
int Verif(unsigned char *req,int taille){
	if(req[0]==93 && req[1]==2){
		uint64_t body_length = byteToNumber(req+2,16);//retourn le num en 64bits (donc 8 octets )

		if(body_length+4==taille){
			int cpt=0;
			//Sinon : 
			for(int i=4;i<taille;i++){
				if(req[i]!=0){
					cpt+=req[i+1]+2;
					i+=req[i+1]+1;
				}
				else if(req[i]==0)
					cpt++;
				else
					break;
			}

			if(cpt==body_length)
				return 1;
		}
	return 0;
	}
}

void decomposeRequest(unsigned char *req, TLV *tlv){
	tlv->type = req[0];
	tlv->length = req[1];

	switch(tlv->type){
		case 2: {

			//Si c'est un hello court
			if(tlv->length==8){
				//On remplit le source id seulement
				for(int i=0;i<8;i++){
					tlv->body.Hello.sourceid[i] = req[2+i];
				}				
			}

			//Si c'est un hello long
			if(tlv->length == 16){

				//on remplit le sourceid & le destId en même temps;
				for(int i=0;i<8;i++){
					tlv->body.Hello.sourceid[i] = req[2+i];
					tlv->body.Hello.destinationid[i] = req[10+i];
				}

			}

			break;
		}

		case 3:{
			//On remplit le TLV avec l'ip
			for(int i=0;i<16;i++){
				tlv->body.Neighbour.ip[i] = req[2+i];
			}
			//On remplit le TLV avec le port
			tlv->body.Neighbour.port[0] = req[18];
			tlv->body.Neighbour.port[1] = req[19];

			break;
		}

		default: {
			break;
		}
	}
}



//Des futures réactions lors des récéptions de TLVs
void checkRecieved (TLV tlv,struct sockaddr_in6 peer){

	switch(tlv.type){


		//Hello
		case 2 :
		{

			Voisin *v;
			if(tlv.length==8){
				if(!isVoisin(p.recent, peer.sin6_addr.s6_addr, peer.sin6_port)){
					v=newVoisin(byteToNumber(tlv.body.Hello.sourceid,64), peer.sin6_addr.s6_addr, peer.sin6_port);
					addVoisin(p.recent,v);
				}

			}
			else if(tlv.length=16){
				if(!isVoisin(p.recent, peer.sin6_addr.s6_addr, peer.sin6_port)){
					v=newVoisin(byteToNumber(tlv.body.Hello.sourceid,64), peer.sin6_addr.s6_addr, peer.sin6_port);
					v->symetrique=1;
					addVoisin(p.recent,v);	
				}
			}
				//Si le voisin est déjà dans la liste des voisins récents on ne fait rien
			break;
		}



		//Neighbor
		case 3 :
		{
			Voisin *voisin;
			uint16_t port = byteToNumber(tlv.body.Neighbour.port,16);
			if(!isVoisin(p.potentiel, tlv.body.Neighbour.ip , peer.sin6_port)){
				voisin = newVoisin(0,tlv.body.Neighbour.ip,port);
				addVoisin(p.potentiel,voisin);	
			}



		//Ce TLV indique que le récepteur a une relation de voisinage symétrique avec un pair à l’adresseIPet écoutant sur le port UDPPort. Les adresses IPv6 sont représentées telles quelles, les adressesIPv4 sont représentées sous formeIPv4-Mapped(dans le préfixe::FFFF:0:0/96).
			break;
		}






		//data
		case 4 :
		{

		//Traiter le Data
			uint64_t id = byteToNumber(tlv.body.Data.senderid,64);
			uint32_t nonce = byteToNumber(tlv.body.Data.nonce,32);
		//vérifier dans la liste de données réçues la pair (id,nonce)

		//Si ce n'est pas le cas, on affiche 
			//printf();


		//Si c'est le cas : 
			TLV tlv;
			newAck(&tlv,id,nonce);


		//Envoyer un ack a l'envoyeur
		//sendRequest(tlv);
			break;
		}
		//ack
		case 5 : 
		//On supprime des voisins à innonder		
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
		printf("Un message d'erreur : \n");
		break;

		//0, PadN ou autres
		default : 
		printf("Un packet à ignorer a été reçu\n");
		//a Ignorer
		break;
	}
}








void createRequest(unsigned char *req, TLV tlv){// Datagramme-->Magic,Version,Body(type,longeur,valeur d'un corp de TLV)
	    //Header
  req[0] = 93; //Magic
  req[1] = 2; //Version
  numberToByte(tlv.length + 2, req+2, 16);
  req[4] = tlv.type;
  req[5] = tlv.length;//(longueur du corp sans type et longueur)
  for(int i = 0;i<tlv.length;i++){
  	req[i+6] = tlv.body.Hello.sourceid[i];
  }


  printf("Requête : ");
  for (int i = 0 ; i < tlv.length + 6 ; i++) { printf("%.2d ", req[i]); }
}










//*************************************************************TLV CONSTRUCTORS **********************************************************************
//**************************************************************************************************************************************

void newPad1(TLV *message){
	message->type = 0;
	message->length = 0;
}

void newPadN(TLV *message){
	message->type = 1;
	message->length = 0;
	memset(message->body.PadN.mbz, 0 , 4078);
}

void newHelloCourt(TLV *message, uint64_t id){

	message->type = 2;
	message->length = 8;
	numberToByte(id,message->body.Hello.sourceid,64);//convertir un nombre ne byte
}	


void newHelloLong(TLV *message, uint64_t sourceid, uint64_t destid){

	message->type = 2;
	message->length = 16;
	numberToByte(sourceid,message->body.Hello.sourceid,64);
	numberToByte(destid,message->body.Hello.destinationid,64);

}	

void newNeighbour(TLV *message, unsigned char *ip, uint16_t port){
	message->type = 3;
	message->length = 18;
	//ON MET L'IP DANS message->body.Neighbour.ip
	numberToByte(port,message->body.Neighbour.port,16);
}

void newData(TLV *message, uint64_t senderid, uint32_t nonce, unsigned char type,  char *data ){
	message->type = 4;
	message->length = 9 + strlen(data);
	numberToByte(senderid,message->body.Data.senderid,32);
	numberToByte(nonce,message->body.Data.nonce,32);
	message->body.Data.type = type;
	strncpy(message->body.Data.data, data, 4065);

}

void newAck(TLV *message, uint64_t senderid, uint64_t nonce){
	message->type = 5;
	message->length = 8;
	numberToByte(senderid,message->body.Data.senderid,32);
	numberToByte(nonce,message->body.Data.nonce,32);
} 

//Convertie un nombre en Unsigned Char Big Endian
void numberToByte(uint64_t number, unsigned char *req,  int size){//Le mode big endian accélère les opérations qui nécessitent de regarder en premier les bits de poids forts comme la recherche du signe, la comparaison de deux entiers et la division.
	int j=0;
	for(int i = size-8; i>=0; i-=8){
		req[j] = number >> i;
		j++;
	}
}



//Convertie un nombre dans un unsigned char (format big endian) en un nombre (uint64)
uint128_t byteToNumber(unsigned char *req, int size){//
	uint128_t num = 0;
	int j=0;
	for(int i=size-8;i>=0;i-=8){
		num += ((uint64_t)req[j] << i);
		j++;
	}

	return num;

}




