#include "structs.h"
#include "requestHandler.h"

Voisin *newVoisin(uint64_t id,unsigned char *ip, uint16_t port){
	
	Voisin *v=(Voisin*)malloc(sizeof(Voisin));
	v->id=id;
	memcpy(v->ip,ip,16);

	v->port=port;
	v->symetrique=0;
	v->next=NULL;
	v->prev=NULL;
	v->anyhello = 0;
	v->longhello = 0;
	return v;
}

void addVoisin(Liste_Voisin *list, Voisin *newVoisin){

		if(list->first==NULL){
			list->first = newVoisin;
			list->last = newVoisin;	
		}
		else{
			list->last->next = newVoisin;
			newVoisin->prev = list->last;
			list->last = newVoisin;
		}
}

void supprimeVoisin(Liste_Voisin *l, unsigned char* ip2){

	Voisin *v;
	if(l->first){
		v=l->first;

		if(memcmp(v->ip,ip2,16)==0){

			l->first=v->next;
			v->next = NULL;
		}

		v=v->next;

		while(v!=NULL){ 
			if(memcmp(v->ip,ip2,16)==0){

				break;
			}
			v=v->next;
		}
		if(v!=NULL){
			if(v->next!=NULL){

				v->prev->next=v->next;
				v->next->prev=v->prev;
				v->next = NULL;
				v->prev = NULL;
			}
			else{

				l->last=v->prev;
				v->prev->next=NULL;
				v->prev=NULL;
			}

		}
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
	p->datas = (Liste_Data*)malloc(sizeof(Liste_Voisin));
	p->datas->first=NULL;
	p->datas->last=NULL;

}

//Vérifie s'il existe un voisin avec l'ip et le port passé en paramétre dans la liste *list
Voisin *isVoisin(Liste_Voisin *list, unsigned char *ip, uint64_t port){
	uint128_t ipNum = byteToNumber(ip,128);

	Voisin *current = list->first;

	while(current!=NULL){
		if(ipNum == byteToNumber(current->ip,128)){
			return current;
		}
		current = current->next;
	}
	return NULL;
}


void afficheListe(Liste_Voisin *list){
	Voisin *current = list->first;
	char str[30];
	while(current!=NULL){
		inet_ntop(AF_INET6, current->ip, str, INET6_ADDRSTRLEN);
		printf("\n(IP : ID :  PORT) : (%s,%lu,%d) \n",str,current->id, current->port);
		current = current->next;
	}

}

//Retourne tout les voisins marqués symétriques dans la liste *list
Liste_Voisin *getSymmetricals(Liste_Voisin *list,int *syms){

	Liste_Voisin *symetriques = (Liste_Voisin*)malloc(sizeof(Liste_Voisin));
	

	if(list->first && list->last){
		symetriques->first=copyOf(list->first);

		symetriques->last = copyOf(list->last);

		Voisin *current = list->first;

		while(current!=NULL){
			//Si c'est un voisin symétrique, on l'ajoute a la liste
			if(current->symetrique!=0){
				symetriques->last->next = copyOf(current);
				symetriques->last = symetriques->last->next;
				if(syms!=NULL) *syms+=1;
			}
			current = current->next;
		}
	}

	else{
		symetriques->first = NULL;
		symetriques->last = NULL;
	}

	return symetriques;
}



//Crée une copie d'un voisin
Voisin *copyOf(Voisin *voisin){
	return newVoisin(voisin->id,voisin->ip,voisin->port);
}


Data *newFloodData (unsigned char *senderid, unsigned char *nonce, char *data){
	Data *newData = (Data*)malloc(sizeof(Data));
	memcpy(newData->senderid,senderid,64);
	memcpy(newData->nonce,nonce,32);
	memcpy(newData->message,data,4065);
	newData->toFlood = getSymmetricals(p.recent,NULL);
	newData->counter = 0;
	newData->next=NULL;
	newData->prev = NULL;
}



Data *recentData(unsigned char *senderid, unsigned char *nonce){
	Data *current = p.datas->first;

	while(current!=NULL){
		if(memcmp(current->senderid,senderid,8)==0 && memcmp(current->nonce,nonce,4)==0)
			return current;
		current = current->next;
	}

	return NULL;
}



void addData(Data *newData){

		Liste_Data *list = p.datas;

		if(list->first==NULL){
			list->first = newData;
			list->last = newData;	
		}
		else{
			list->last->next = newData;
			newData->prev = list->last;
			list->last = newData;
		}

}



// void afficheDatas(Liste_Data *list){
// 	Data *current = list->first;
// 	while(current!=NULL){
// 		printf("\n(ID : Nonce :  Message ) : (%llu,%llu, %s) \n",current->senderid, current->nonce, current->message);
// 		current = current->next;
// 	}

// }


int isEmpty(Liste_Voisin *list){
	return (list->first==NULL)?1:0;
}