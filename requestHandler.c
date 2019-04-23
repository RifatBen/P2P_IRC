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
				memcpy(tlv->body.Hello.sourceid,req+2,8);
			}

			//Si c'est un hello long
			if(tlv->length == 16){
				memcpy(tlv->body.Hello.sourceid,req+2,8);
				memcpy(tlv->body.Hello.destinationid,req+2,8);
			}

			break;
		}

		case 3:{
			//On remplit le TLV avec l'ip
			memcpy(tlv->body.Neighbour.ip,req+2,16);
			//On remplit le TLV avec le port
			memcpy(tlv->body.Neighbour.port,req+18,2);

			break;
		}


		case 4:{
			memcpy(tlv->body.Data.senderid,req+2,8);
			memcpy(tlv->body.Data.nonce, req+10,4);
			tlv->body.Data.type=req[14];
			memset(tlv->body.Data.data,0,4065);
			memcpy(tlv->body.Data.data,req+15,tlv->length-13);
			break;
		}

		case 6:{
			tlv->body.GoAway.code = req[2];
			strcpy(tlv->body.GoAway.message,(char*)req+3);
			break;
		}

		case 7:{
			strcpy(tlv->body.Warning.message,(char*)req+3);
			break;
		}

		default: {
			break;
		}
	}
}



//Des futures réactions lors des récéptions de TLVs
void checkRecieved (int s,TLV tlv,struct sockaddr_in6 peer){

	switch(tlv.type){


		//Hello
		case 2 :
		{

			Voisin *v=isVoisin(p.recent, peer.sin6_addr.s6_addr, peer.sin6_port);
			if(tlv.length==8){
			//Si ce n'est pas un voisin récent
				if(v==NULL){

					v=isVoisin(p.potentiel, peer.sin6_addr.s6_addr, peer.sin6_port);
				//Si c'est un voisin potentiel
					if(v!=NULL){
						//On met a jour son id si on ne l'avait pas
						v->id=byteToNumber(tlv.body.Hello.sourceid,64);
						//On on le retire des voisins potentiel
							supprimeVoisin(p.potentiel, v->ip);
					}

				//Si ce n'est pas un voisin potentiel
					else
						v=newVoisin(byteToNumber(tlv.body.Hello.sourceid,64), peer.sin6_addr.s6_addr, peer.sin6_port);
						
			//Dans tous les cas : on met à jours ses temps et on l'ajoute aux recents
					addVoisin(p.recent,v);
					//v->LESTEMPS
					//*****************************************************************************************************
				}

				//Si c'est un voisin récent
				else{
					//On met a jour ses temps
				}

			}

			//Si c'rest un hello Long
			else if(tlv.length=16){
				//Si c'est un voisin recent

						
				
				//Si ce n'est pas un voisin récent
				if(v==NULL){
					v=isVoisin(p.potentiel, peer.sin6_addr.s6_addr, peer.sin6_port);
					//Si c'est un voisin potentiel
					if(v!=NULL){
						uint64_t destinationid =byteToNumber(tlv.body.Hello.destinationid,64);
						//Si le hello nous est destiné : 
						if(destinationid == p.id){
							//on met a jour son id
							v->id=byteToNumber(tlv.body.Hello.sourceid,64);
							//On met a jour ses temps
							//v->LESTEMPS
							//On le supprime des potentiel
							supprimeVoisin(p.potentiel,v->ip);
						}
						else{
							//Sinon 
							//GoAway ce n'est pas mon id!
						}
					}

					//S'il n'est ni voisin potentiel, ni récent, alors on le crée
					else
						v=newVoisin(byteToNumber(tlv.body.Hello.sourceid,64), peer.sin6_addr.s6_addr, peer.sin6_port);

					//On l'ajoute aux voisins récents et on met a jour sa symétrie
					addVoisin(p.recent,v);
					v->symetrique=1;
				}
				// printf("l'id : %llu",byteToNumber(tlv.body.Hello.sourceid,64));
			}
			break;
		}



		//Neighbour
		case 3 :
		{
			Voisin *voisin= isVoisin(p.recent, peer.sin6_addr.s6_addr, peer.sin6_port);
			uint16_t port = byteToNumber(tlv.body.Neighbour.port,16);
			if(voisin==NULL){
				voisin = newVoisin(0,tlv.body.Neighbour.ip,port);
				addVoisin(p.potentiel,voisin);	
			}



		//Ce TLV indique que le récepteur a une relation de voisinage symétrique avec un pair à l’adresseIPet écoutant sur le port UDP Port.
			break;
		}






		//data
		case 4 :
		{	
			//Traiter le Data 

			Data *newData = recentData(tlv.body.Data.senderid,tlv.body.Data.nonce);
			//vérifier dans la liste de données réçues la pair (id,nonce)
				//Si ce n'est pas le cas, on affiche 
			if(newData == NULL){
				//Si le type de Data est 0, on affiche
				if(tlv.body.Data.type==0)
					printf("\"%s\"\n\n",tlv.body.Data.data);
				newData = newFloodData(tlv.body.Data.senderid,tlv.body.Data.nonce,tlv.body.Data.data);
				//On supprime l'emetteur du message Data
				addData(newData);
			}
			//Envoyer un ack a l'emetteur
			TLV newTlv;
			newAck(&newTlv, tlv.body.Data.senderid, tlv.body.Data.nonce);																	
			sendRequest(s,peer,&newTlv);
			//On supprime l'emetteur du message Data
			supprimeVoisin(newData->toFlood, peer.sin6_addr.s6_addr);
			//Innonder le message aux voisins ssymétriques
			flood(s,newData);
			break;
		}




		//ack
		case 5 : {
		//On vérifie si c'est un Ack d'une donnée qu'on a
			Data *newData = recentData(tlv.body.Ack.senderid,tlv.body.Ack.nonce);
			if(newData != NULL){
				printf("dans le if");
				supprimeVoisin(newData->toFlood,peer.sin6_addr.s6_addr);
			}
			else{
				printf("dans le else");
			}

			break;
		}	


		//Goaway
		case 6 : 
		//Réagir en fonction du code reçu
		printf("GoAway %d : %s", tlv.body.GoAway.code, tlv.body.GoAway.message);
		//Si code == 1 : On retire l'emetteur des voisins
		if(tlv.body.GoAway.code == 1){
			Voisin *voisin = isVoisin(p.recent, peer.sin6_addr.s6_addr, peer.sin6_port);
			if(voisin){
				supprimeVoisin(p.recent,peer.sin6_addr.s6_addr);
				addVoisin(p.potentiel,voisin);
			}
		}	
		//Si code == 2 || 3: Envoyer un hello long
		if(tlv.body.GoAway.code == 2 || tlv.body.GoAway.code == 3){
			TLV tlv;
			Voisin *v = isVoisin(p.recent,peer.sin6_addr.s6_addr, peer.sin6_port);
			if(v!=NULL){
				newHelloLong(&tlv,p.id,  v->id);
				sendRequest(s, peer, &tlv);
			}
		}

		//Retirer le recepteur des voisins symmétriques/ou retirer de la liste tout court
		//LE garder dans les potentiels voisins
		break;

		//Warning
		case 7 : 
		printf("Message warning : %s",tlv.body.Warning.message);
		printf("Un message d'erreur : \n");
		break;

		//0, PadN ou autres
		default : 
		//a Ignorer
		break;
	}
}







void flood(int s,Data *data){
	TLV tlv;
	newData(&tlv,data->senderid, data->nonce, data->type, data->message);
	Voisin *v;
	struct sockaddr_in6 peer;
	for(int i=1;i<=5;i++){
		//Si la liste est vide, on sort de la boucle et on arrête l'innondation
		if(isEmpty(data->toFlood))
			break;
		
		sleep(pow(2,i-1));

		v=data->toFlood->first;
		
		//Innondation du Data
		while(v!=NULL){
			peer.sin6_family = AF_INET6;
			peer.sin6_port = v->port;
			memcpy(peer.sin6_addr.s6_addr, v->ip,16);
			sendRequest(s,peer,&tlv);
			v=v->next;
		}



	}

	//Envoyer GoAway code 2 a tt ceux qui restent
	v=data->toFlood->first;
	while(v!=NULL){
		peer.sin6_family = AF_INET6;
		peer.sin6_port = v->port;
		memcpy(peer.sin6_addr.s6_addr, v->ip,16);
		newGoAway(&tlv,2,"Didn't recieve Ack from you!");
		sendRequest(s,peer,&tlv);
			//supprimer des voisins récent
		supprimeVoisin(p.recent, v->ip);
			//Ajouter dans les voisins potentiel
		addVoisin(p.potentiel, v);
		v=v->next;
	}
	
}



int createRequest(unsigned char *req, TLV *tlv, int nbrTLV){// Datagramme-->Magic,Version,Body(type,longeur,valeur d'un corp de TLV)
		
	    //Header
  req[0] = 93; //Magic
  req[1] = 2; //Version

  int alltlvlen= 0;

  for(int i=0;i<nbrTLV;i++){
  	if(tlv[i].type==0)
  		alltlvlen += 1;
  	else
  		alltlvlen +=  tlv[i].length +2;
  }
  
  numberToByte(alltlvlen, req+2, 16);
  int j=0;
  for(int i=4; i<alltlvlen+4;){
  	if(tlv[j].type!=0){
  		req[i++]=tlv[j].type;
  		req[i++]=tlv[j].length;

  		switch(tlv[j].type){
  			case 1:{
  				for(int k=0;k<tlv[j].length;k++)
  					req[i++]=tlv[j].body.PadN.mbz[k];


  				break;
  			}

  			case 2:{
  				if(tlv[j].length==8){
  					for(int k=0;k<8;k++)
  						req[i++]=tlv[j].body.Hello.sourceid[k];
  				}
  				if(tlv[j].length==16){
  					for(int k=0;k<8;k++)
  						req[i++]=tlv[j].body.Hello.sourceid[k];
  					for(int k=0;k<8;k++)
  						req[i++]=tlv[j].body.Hello.destinationid[k];
  				}
  				break;
  			}

  			case 3:{
  				for(int k=0;k<tlv[j].length-2;k++)
  					req[i++]=tlv[j].body.Neighbour.ip[k];
  				req[i++]=tlv[j].body.Neighbour.port[0];
  				req[i++]=tlv[j].body.Neighbour.port[1];
  				break;
  			}

  			case 4:{
  				for(int k=0;k<8;k++)
  					req[i++]=tlv[j].body.Data.senderid[k];

  				for(int k=0;k<4;k++)
  					req[i++]=tlv[j].body.Data.nonce[k];

  				req[i++]=tlv[j].body.Data.type;
  				for(int k=0;k<strlen(tlv[j].body.Data.data);k++)
  					req[i++]=tlv[j].body.Data.data[k];

  				break;
  			}
  			case 5:{
  				for(int k=0;k<8;k++)
  					req[i++]=tlv[j].body.Ack.senderid[k];

  				for(int k=0;k<4;k++)
  					req[i++]=tlv[j].body.Ack.nonce[k];
  				break;
  			}
  			case 6:{
  				req[i++]=tlv[j].body.GoAway.code;
  				for(int k=0;k<strlen(tlv[j].body.GoAway.message);k++)
  					req[i++]=tlv[j].body.GoAway.message[k];
  			}

  		}
  		j++;	
  	}

  	//Si c'est un TLV Pad1
  	else
  		req[i++]=tlv[j].type;

  		//On passe au TLV suivant
  	j++;
  }
  return alltlvlen+4;
}




int sendRequest(int s,struct sockaddr_in6 peer, TLV *tlvs){
	unsigned char req[4096] = {0};
	int lenreq = createRequest(req,tlvs,1);
	
	// char str[4078];
	// 		strcpy(str,(char*)req);
	// 		printf("\n\n\nLA STR : %s \n\n\n\n",str);
	int rc=sendto(s,req,lenreq,0, (struct sockaddr*)&peer,sizeof(struct sockaddr_in6)) ;
	// printf("\nRequête envoyée : ");

	// for(int i=0;i<lenreq;i++){printf("%c ",req[i]);}
	// 	printf("\n");
	if(rc < 0) {
		fprintf(stderr,"Error recev\n");
		return 0;
	}
	else
		return 1;
}






void Envoi_Court(int s,Liste_Voisin *l){
	TLV tlv;
	newHelloCourt(&tlv,1024);
	Voisin *v;
	v=l->first;
	struct sockaddr_in6 peer;
	while(v!=NULL){//ENvoi d'un meme Hello Court à chaque voisin
		peer.sin6_family = AF_INET6;
	peer.sin6_port = v->port;
	memcpy(peer.sin6_addr.s6_addr, v->ip,16);
	sendRequest(s,peer,&tlv);
	v=v->next;
}

}



void Envoi_Long(int s,Liste_Voisin *l){
	TLV tlv;
	Voisin *v;
	v=l->first;

	struct sockaddr_in6 peer;
	while(v!=NULL){//envoi de Hello long different à chaque voisin
		peer.sin6_family = AF_INET6;
		peer.sin6_port = v->port;
		memcpy(peer.sin6_addr.s6_addr, v->ip,16);
		newHelloLong(&tlv,p.id,v->id);
		sendRequest(s,peer,&tlv);
		v=v->next;
	}
	
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
	for(int i=0;i<16;i++)
		message->body.Neighbour.ip[i]=ip[i];
	numberToByte(port,message->body.Neighbour.port,16);
}

void newData(TLV *message, unsigned char * senderid, unsigned char * nonce, unsigned char type,  char *data ){
	message->type = 4;
	message->length = 13 + strlen(data);
	memcpy(message->body.Data.senderid,senderid,8);
	memcpy(message->body.Data.nonce,nonce,4);
	message->body.Data.type = type;
	strncpy(message->body.Data.data, data, strlen(data));

}

void newAck(TLV *message, unsigned char * senderid, unsigned char * nonce){
	message->type = 5;
	message->length = 8+4;
	memcpy(message->body.Ack.senderid,senderid,8);
	memcpy(message->body.Ack.nonce,nonce,4);
} 


void newGoAway(TLV *message, unsigned char code, char *msg){
	message->type = 6;
	message->length = 1+strlen(msg);
	message->body.GoAway.code = code;
	strncpy(message->body.GoAway.message, msg, strlen(msg));	
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




