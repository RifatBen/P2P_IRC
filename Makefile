CC = irc
SOURCES = main.c structs.c requestHandler.c parser.c
FLAGS = -lpthread -lm -lreadline -w 

all : $(CC)

$(CC) : 
	gcc $(SOURCES) $(FLAGS) -o $@
clean : 
	rm $(CC)