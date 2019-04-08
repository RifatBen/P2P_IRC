#ifndef structs_h
#define structs_h

#include "includes.h"

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
	uint64_t id[8];
	unsigned char port[5];
	short symetrique;

	time_t shorthello;
	time_t longhello;
	struct Voisins *next;

}Voisins;


typedef struct Peer{
	unsigned char id[8];
	struct Voisins *potentiel;
	struct Voisins *recent;
}Peer;


Voisins *newVoisin(uint8_t id, uint8_t port);

#endif