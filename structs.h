#ifndef structs_h
#define structs_h

#include "includes.h"


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
				unsigned char type;
				char data[4065];
			}Data;

			struct{
				unsigned char senderid[8];
				unsigned char nonce[4];
			}Ack;

			struct{
				unsigned char code;
				char message[4077];
			}GoAway;

			struct{
				char message[4078];
			}Warning;

		}body;
}TLV;


typedef struct Voisin{
	unsigned char ip[16];
	uint64_t id;
	uint16_t port;
	short symetrique;

	time_t shorthello;
	time_t longhello;
	struct Voisin *next;
	struct Voisin *prev;   


}Voisin;


typedef struct Liste_Voisin{
	Voisin *first;
	Voisin *last;
}Liste_Voisin;

typedef struct Data{
	unsigned char senderid[8];
	unsigned char nonce[4];
	unsigned char type;
	char message[4065];


	Liste_Voisin *toFlood;
	short counter;
	struct Data *next;
	struct Data *prev;
}Data;

typedef struct Liste_Data{
	Data *first;
	Data *last;
}Liste_Data;

typedef struct Peer{
	uint64_t id;
	Liste_Voisin *potentiel;
	Liste_Voisin *recent;
	Liste_Data *datas;
}Peer;



extern Peer p;


Voisin *newVoisin(uint64_t id,unsigned char *ip, uint16_t port);

void addVoisin(Liste_Voisin *list, Voisin *newVoisin);

void initPeer(Peer *p);

Voisin *isVoisin(Liste_Voisin *list, unsigned char *ip, uint64_t port);

void supprimeVoisin(Liste_Voisin *l,unsigned char* ip2);

void afficheListe(Liste_Voisin *list);

Liste_Voisin *getSymmetricals(Liste_Voisin *list);

Voisin *copyOf(Voisin *voisin);

Data *recentData(unsigned char *senderid, unsigned char *nonce);

Data *newFloodData (unsigned char *senderid, unsigned char *nonce, unsigned char *data);

void addData(Data *newData);


void afficheDatas(Liste_Data *list);
#endif