#include "server.h"

int main(int argc, char *argv[]) {
    server s;
    // All ports below 1024 are RESERVED (unless you’re the superuser)! You can have any port number above that, right up to 65535
    char PORT[] = "3490"; // set default value of PORT

	if (argc < 2) {
		printf("WILL USE PORTNUMBER: %s\n", PORT);
		printf("IF YOU WANT TO CHANGE THE DEFAULT RUN THE FOLLOWING\n");
        printf("./my_server portNumber\n");
        printf("THANKS :)\n--------------------------------------------------------------------------------\n");
	}
    if (argc >= 2){
        strcpy(PORT, argv[1]); // get port number from user
    }

    s.run(PORT);
}