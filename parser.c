#include "parser.h"

	//Fonction qui l'absence d'une fonction
int error(char *cmd){
	printf("%s command not found! Check the help command for more details about the commands\n",cmd);
	return 0;
}


	// Fonction qui prend une line, et la décompose en plusieurs arguments
char **line_split(char *line){
	char *linedup = strdup(line);
	char *arg = strtok(linedup," ");
	char **args = calloc(SIZE , sizeof(char*));

	int i = 0, buffer = SIZE;

	if (!args) {
		perror("calloc error");
		exit(EXIT_FAILURE);
	}

	while (arg != NULL) {
		args[i++] = arg;


	      //S'il y'a dépassement de la taille on resize
		if (i >= buffer) {
			buffer += SIZE;
			args = realloc(args, buffer * sizeof(char*));

			if (!args) {
				perror("realloc error");
				exit(EXIT_FAILURE);
			}
		}
	      //on passe au prochain argument
		arg = strtok(NULL, " ");
	}
	    //On determine la fin des arguments par un NULL
	args[i] = NULL;

	return args;
}


	//Fonction qui prend une ligne de commande et essaye de l'executer
int traitement_ligne_de_commande(char *line){
	char **args = line_split(line);

	if(args[0]!=NULL){

		if(strcmp("/send",args[0])==0){
			if(!args[1]){
				printf("Error, missing message for send command\n");
				return 0;
			}
		}
		return exec_command(args);

	}

	return 0;
}

int exec_command(char **args){

	int nbr_fct_intern = funcs_num();
	int i;
	for (i=0;i<nbr_fct_intern; i++) {
		if(strcmp(args[0], func_names[i]) == 0) {
			return (*funcs[i])(args);
		}
	}
	return error(args[0]);

}



int irc_send(char **args){
	char message[4066]="";

		strcat(message,args[1]);
	for(int i=2; args[i]; i++){
		strcat(message, " ");
		strcat(message,args[i]);
	}

	sendData(s,nick,message);

	fprintf(stderr,"%s : %s\n",nick,message);

	return 1;
}

int irc_nick(char **args){
	strcpy(nick,args[1]);
	printf("Pseudo changé en : %s\n",nick);
	return 1;
}

int irc_exit(char **args){
	exit(1);
}