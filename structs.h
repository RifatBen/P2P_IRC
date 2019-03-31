#ifndef structs_h
#define structs_h

#include "stdio.h"
#include "time.h"
#include "stdlib.h"
#include "string.h"
#include <arpa/inet.h>


typedef struct TLV {
	char type;
	char length;
	char body[4078];
}TLV;

typedef struct Header {
	char magic;
	char version;
	char bodylen[16];
}Header;

typedef struct Datagramme{
	struct header *head;
	struct TLV *body;
}Datagramme;


typedef struct Voisins{
	char id[8];
	char port[5];
	int symetrique;
	//La taille/type/format de ces deux là vont devoir être changé, car ils sont censé représenté un temps
	//Mais normalement ça sera grâce au time.h
	time_t shorthello;
	time_t longhello;
	struct Voisins *next;

}Voisins;


typedef struct Peer{
	char id[8];
	struct Voisins *potentiel;
	struct Voisins *recent;
}Peer;


TLV *newTLV(int type, int length, char *body);
Voisins *newVoisin(int id, int port);

#endif