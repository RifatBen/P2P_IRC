#define _GNU_SOURCE

#include "includes.h"
#include "structs.h"
#include "requestHandler.h"

#define BUF_SIZE 1024
#define REQ_SIZE 14


void fillSocket(struct sockaddr_in6 * peer);

Peer p;


int main(){


initPeer(&p);

	unsigned char req[REQ_SIZE] = {0};

	unsigned char buf [BUF_SIZE] = {0}; // Buffer de réception



	int s = socket(AF_INET6, SOCK_DGRAM, 0);
	if (s < 0) {
		fprintf(stderr, "Bug socket\n");
		exit(1);
	}

	struct sockaddr_in6 sin6 = {0}; 

  // Socket de réception de l'adresse du pair
	struct sockaddr_in6 peer = {0};


	int peer_size = sizeof(struct sockaddr_in6);

  // Définition de la socket
  sin6.sin6_family = AF_INET6; // Adresse IPV6
  sin6.sin6_port = htons(1212); // Port local
  
  int rc = bind(s, &sin6, sizeof(sin6));
  if (rc < 0) {
  	fprintf(stderr,"Error bind\n");
  	exit(1);
  }

  //Remplit l'adresse client
  fillSocket(&peer);

  //On crée notre TLV Hello court (Ou nimporte quel autre TLV grace aux fonctions dont on dispose)
  TLV tlv;
  newHelloCourt(&tlv,1024);

  //On crée la requête avec notre tlv
  createRequest(req,tlv);

  //TLV
  int lenreq = sizeof(req)/sizeof(unsigned char);

   // Et on l'envoie à un pair
  while(1) {
  sendto(s,req,lenreq, 0, &peer, peer_size);
  printf("\nEnvoi effectué\n"); 

    // Réception d'une requête
    rc = recvfrom(s, buf, BUF_SIZE,0,&peer, &peer_size);
    if(rc < 0) {
      fprintf(stderr,"Error recev\n");
      exit(1);
    }

    // Affichage de la requête reçu
    printf("\nMessage reçu\n");
    for (int i = 0 ; i < rc ; i++) { printf("%.2d ", buf[i]); }
      //On vérifie si la requête est bien formée
      
      if(Verif(buf,rc,(int)buf[5])){
        //Décomposer la requete reçue, c'est à dire faire transformer la requete unsigned char en TLV (byteToNumber & numberToByte)  <---------- dans une fonction
        TLV tlv;

        decomposeRequest(buf,&tlv);
        //On analyse maintenant la requête obtenue
        checkRecieved(tlv, peer);
        afficheListe(p.recent);
      }

      //Sinon, on ignore


  }

  return EXIT_SUCCESS;
}



void fillSocket(struct sockaddr_in6 * peer) {
	int rc = 0;
	struct addrinfo hints = {0};
	struct addrinfo *r;
  hints.ai_family = AF_INET6; // Oblige l'IPV6
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = (AI_V4MAPPED | AI_ALL);
  // Remplace gethostbyname _
  rc = getaddrinfo("jch.irif.fr", "1212", &hints, &r);
  // Ici tu peux faire une itération (cf man)
  struct addrinfo *p = r;
  if (rc < 0 || p == NULL) {
  	fprintf(stderr, "Bug socket\n");
  	freeaddrinfo(r);
  	exit(1);
  }

  // On copie l'adresse dans le pair
  memmove(peer,p->ai_addr, sizeof(struct sockaddr_in6));
  freeaddrinfo(r);
}