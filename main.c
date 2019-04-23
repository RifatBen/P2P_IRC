#define _GNU_SOURCE

#include "includes.h"
#include "structs.h"
#include "requestHandler.h"

#define BUF_SIZE 4096
#define REQ_SIZE 4096

void initSocket(int s, struct sockaddr_in6 *sin6);

void fillSocket(struct sockaddr_in6 * peer);

Peer p;


int main(){


	initPeer(&p);


	unsigned char buf [BUF_SIZE] = {0}; // Buffer de réception



	int s = socket(AF_INET6, SOCK_DGRAM, 0);
	if (s < 0) {
		fprintf(stderr, "Bug socket\n");
		exit(1);
	}

	// fcntl(s, F_SETFL, fcntl(s, F_GETFL) | O_NONBLOCK);

	struct sockaddr_in6 sin6 = {0}; 

  // Socket de réception de l'adresse du pair
	struct sockaddr_in6 peer = {0};
	int peer_size = sizeof(struct sockaddr_in6);



	initSocket(s,&sin6);


  //Remplit l'adresse client du prof
	fillSocket(&peer);

  //On crée notre TLV Hello court (Ou nimporte quel autre TLV grace aux fonctions dont on dispose)
	TLV tlv,tlv2;

	uint128_t  idProf = 11362007626057429800;
	// newHelloLong(&tlv, p.id, idProf);
	unsigned char nonce[2];
	unsigned char ide[8];
	numberToByte(1094,nonce,16);
	numberToByte(1092,ide,64);
	newData(&tlv,ide,nonce,0, "Quelque chose d'interessant");
  //On crée un tableau de TLV qu'on remplit avec les TLV qu'on crée
	TLV tlvtab[2] = {0};
	tlvtab[0]=tlv;


// //On crée un faux hello long
// 	uint16_t port = 1111;
// 	unsigned char ip[sizeof(struct sockaddr_in6)];
// 	char str[INET6_ADDRSTRLEN] = "2a01:e35:8a6e:d2a0:2954:4706:a157:4840";
//   //On convertit l'adresse ip en big endian
// 	inet_pton(AF_INET6, str, ip);

//   //On crée le TLV neighbour, puis on l'ajoute au tableau
// 	newNeighbour(&tlv2,ip,port);
// 	tlvtab[1] = tlv2;
  //On crée la requête avec nos tlv
	// createRequest(req,tlvtab,2);
   // Et on l'envoie à un pair
	while(1) {
  	//On envoie la requete avec les TLV qu'on a créé
			sendRequest(s,peer,tlvtab);


  // sendto(s,req,lenreq, 0, &peer, peer_size);
  // printf("\nEnvoi effectué\n"); 

    // Réception d'une requête
		int rc = recvfrom(s, buf, BUF_SIZE,0,&peer, &peer_size);
		if(rc < 0) {
			fprintf(stderr,"Error recev\n");
			exit(1);
		}
		if(rc<=4096){

	    // Affichage de la requête reçu
			printf("\nMessage reçu\n");
			for (int i = 0 ; i < rc ; i++) { printf("%.2d ", buf[i]); }
	      //On vérifie si la requête est bien formée
				if(Verif(buf,rc)){
					printf("Packet bien forme\n");
					int cpt=0;
	        //Décomposer la requete reçue, c'est à dire faire transformer la requete unsigned char en TLV (byteToNumber & numberToByte) 
					for(int i=4;i<rc;i++){
						TLV recvTLV;
						cpt++;
						decomposeRequest(buf+i,&recvTLV);
						checkRecieved(s,recvTLV,peer);
						i+=buf[i+1]+1;
					}
					printf("Il y'a %d TLV.\n",cpt);
					// afficheListe(p.recent);
					// afficheListe(p.potentiel);
					// afficheDatas(p.datas);
				}

	      //Sinon, on ignore


			}
			memset(buf,0,BUF_SIZE);
		}

		return EXIT_SUCCESS;
	}



	void initSocket(int s, struct sockaddr_in6 *sin6){
	
  // Définition de la socket
  sin6->sin6_family = AF_INET6; // Adresse IPV6
  sin6->sin6_port = htons(1212); // Port local
  
  int rc = bind(s, sin6, sizeof(*sin6));
  if (rc < 0) {
  	fprintf(stderr,"Error bind\n");
  	exit(1);
  }

}

void fillSocket(struct sockaddr_in6 * peer) {
	int rc=0;
	struct addrinfo hints = {0};
	struct addrinfo *r;
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = (AI_V4MAPPED | AI_ALL);
  	rc = getaddrinfo("jch.irif.fr", "1212", &hints, &r);
  	struct addrinfo *p = r;
	if (rc < 0 || p == NULL) {
		fprintf(stderr, "Bug socket\n");
		exit(1);
	}

  // On copie l'adresse dans le pair
	memmove(peer,p->ai_addr, sizeof(struct sockaddr_in6));
	freeaddrinfo(r);
}