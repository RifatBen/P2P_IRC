#ifndef parser_h
#define parser_h


	#include "stdio.h"
	#include "stdlib.h"
	#include "string.h"
	#include "requestHandler.h"

extern int SIZE;
int s;
char nick[15];

//Pointeur sur les fonctions  
extern int (*funcs[]) (char **);

//fonction qui compte le Nombre de commandes
extern int funcs_num();

//Tableau contenant le nom des function à appliquer (ex : nick..)
extern char *func_names[];


//Fonction qui prend une ligne de commande et essaye de l'executer
int traitement_ligne_de_commande(char *line);

// Fonction qui prend une line, et la décompose en plusieurs arguments
char **line_split(char *line);

//Fonction qui l'absence d'une fonction
int error(char *cmd);


// fonction qui execute une commande grace a une liste d'arguments passés en parametre
int exec_command(char **args);

int irc_send(char **args);
#endif
