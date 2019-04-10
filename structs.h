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


typedef struct Voisins{
	unsigned char ip[16];
	uint64_t id;
	uint16_t port;
	short symetrique;

	time_t shorthello;
	time_t longhello;
	struct Voisins *next;
	struct Voisins *last;

}Voisins;


typedef struct Peer{
	uint128_t id;
	struct Voisins *potentiel;
	struct Voisins *recent;
}Peer;



extern Peer p;


Voisins *newVoisin(uint8_t id, uint8_t port);

void addVoisin(Voisins *list, Voisins *newVoisin);

void initPeer(Peer *p);

#endif