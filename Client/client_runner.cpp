#include "client.h"

int main(int argc, char *argv[]) {
    client c;

    char PORT[] = "3490"; // set default value of PORT
    char HOSTNAME[] = "localhost"; // set default value of HOSTNAME
    char COMMANDFILE[] = "commands.txt"; // set default value of COMMANDFILE

    if (argc < 4) {
        printf("WILL USE HOSTNAME: %s\nPORTNUMBER: %s\nCOMMANDFILE: %s\n", HOSTNAME, PORT, COMMANDFILE);
        printf("IF YOU WANT TO CHANGE ONE OF THE DEFAULT RUN THE FOLLOWING\n");
        printf("./my_client hostname portNumber commands\n");
        printf("THANKS :)\n--------------------------------------------------------------------------------\n");
    }

	if (argc == 4) {
        strcpy(COMMANDFILE, argv[3]); // get command file from user
        strcpy(PORT, argv[2]); // get port number from user
        strcpy(HOSTNAME, argv[1]); // get host name from user
	}

    c.run(HOSTNAME, PORT, COMMANDFILE);
}