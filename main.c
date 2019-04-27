#define _GNU_SOURCE

#include "includes.h"
#include "structs.h"
#include "requestHandler.h"

#define BUF_SIZE 4096
#define REQ_SIZE 4096

void initSocket(int s, struct sockaddr_in6 *sin6);

void fillSocket(struct sockaddr_in6 * peer);

Peer p;

pthread_mutex_t lock; 

int main(){

	pthread_t t; 
	initPeer(&p);

	//Init le lock
	if (pthread_mutex_init(&lock, NULL) != 0){ 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
  

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

	//Ajout du client Juliusz aux voisins potentiels
	addVoisin(p.potentiel,newVoisin(0, peer.sin6_addr.s6_addr, peer.sin6_port));
  //On crée notre TLV Hello court (Ou nimporte quel autre TLV grace aux fonctions dont on dispose)
	TLV tlv;

	newNeighbour(&tlv,peer.sin6_addr.s6_addr, peer.sin6_port);


	unsigned char nonce[2];
	unsigned char ide[8];
	numberToByte(10949,nonce,16);
	numberToByte(1092,ide,64);
	// newData(&tlv,ide,nonce,0, "Tarass : Quelque chose d'interessant");
	pthread_create(&t,NULL,&maintenanceVoisins,(void *)s);
    

	while(1) {
  	//On envoie la requete avec les TLV qu'on a créé

			// sendRequest(s,peer,&tlv,1);	


    // Réception d'une requête
		int rc = recvfrom(s, buf, BUF_SIZE,0,&peer, &peer_size);
		if(rc < 0) {
			fprintf(stderr,"Error recev,%d\n",rc);
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
						pthread_args args = {s,recvTLV,peer};
 						//pthread_create(&t,NULL,&checkRecieved,(void *)&args);
						checkRecieved((void*)&args);
						i+=buf[i+1]+1;
					}
					printf("Il y'a %d TLV.\n",cpt);
					printf("LES RECENTS : \n");
					afficheListe(p.recent);
					printf("LES POTENTEISL : \n");
					afficheListe(p.potentiel);
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
  	rc = getaddrinfo("94.239.109.6", "2020", &hints, &r);
  	struct addrinfo *p = r;
	if (rc < 0 || p == NULL) {
		fprintf(stderr, "Bug socket\n");
		exit(1);
	}

  // On copie l'adresse dans le pair
	memmove(peer,p->ai_addr, sizeof(struct sockaddr_in6));
	freeaddrinfo(r);
}