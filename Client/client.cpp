/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>


unsigned int microsecond = 1000000;

// #define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 1000000 // max number of bytes we can get at once 
#define MAXFILEPATHSIZE 50 // max number of bytes we can get at once 
#define MAXPORTNUMBSIZE 20 // max number of bytes we can get at once 
#define MAXHOSTNAMESIZE 50 // max number of bytes we can get at once 
#define GET_REQUEST "GET"
#define POST_REQUEST "POST"
#define CONTENT_LENGTH "Content-Length: "
#define ENDREQUEST "\r\n"

#define BACKLOG 5	 // how many pending connections queue will hold

#define NOTFOUND_RESPONSE "HTTP/1.1 404 Not Found\r\n"
#define OK_RESPONSE "HTTP/1.1 200 OK\r\n"
#define END_OF_TEXT "\0"

 /* GET / HTTP/1.1
    Host: localhost:3490
    User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:94.0) Gecko/20100101 Firefox/94.0
    Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,;q=0.8
    Accept-Language: en-US,en;q=0.5
    Accept-Encoding: gzip, deflate
    Connection: keep-alive
    Upgrade-Insecure-Requests: 1
    Sec-Fetch-Dest: document
    Sec-Fetch-Mode: navigate
    Sec-Fetch-Site: none
    Sec-Fetch-User: ?1 */

 /* GET /somedir/page.html HTTP/1.1
    Host: www.someschool.edu
    User-agent: Mozilla/4.0
    Connection: close
    Accept-language:fr */

struct messege_content {

    char request[8]; // GET or Post
    char file_path[MAXFILEPATHSIZE];
    char host_name[MAXHOSTNAMESIZE];
    char port_number[MAXPORTNUMBSIZE];
    char request_msg[MAXDATASIZE]; // Whole request messege

};
char buffer[MAXDATASIZE];

std::string read_from_file (std::string path)
{
    std::ifstream MyReadFile(path);
    if (MyReadFile.good()) {
        std::string myText; 
        std::string totText;
        totText = "";
        while (getline (MyReadFile, myText))
            totText += myText  + "\n"; // Write to the file
        if (totText.length() >= 1)
			totText.substr(0, totText.length()-1);
        MyReadFile.close(); // Close the file

        return totText;
    }
    printf("FILE NOT FOUND: %s\n", path.c_str());
    return "";
}

void save_recieved_content(char *buffer, char *file_path) {
    std::string s = buffer;
    if (strcmp(buffer, NOTFOUND_RESPONSE)) {
        std::string path = "public/" + std::string(file_path);
        std::ofstream MyFile(path);
        s = s.substr(s.find(CONTENT_LENGTH) + std::string(CONTENT_LENGTH).length());
        int cLen = stoi(s.substr(0,s.find("\n")));
        s = s.substr(s.find("\n")+3, cLen-1);
        printf("Writitng to path %s\n",path.c_str());
        MyFile << s; // Write to the file
        MyFile.close(); // Close the file
    }
}

struct messege_content request_preprocessing(char command[])
{
    // client_get file-path host-name (port-number)
    // client_post file-path host-name (port-number)

    int lArgs = 0;
    char *args[5];
    char *token = strtok(command, " "); 
    while (token != NULL) { 
        args[lArgs++] = token;
        token = strtok(NULL, " "); 
    } 

    args[lArgs] = NULL;
    struct messege_content cmd;
    char extra_data[MAXDATASIZE];
    char request[MAXDATASIZE];
    char *host_name = args[2], *port_number = args[3];
    char *file_path = args[1];

    if (strcmp(args[0] ,"client_get") == 0) {
        strcpy(cmd.request, GET_REQUEST);
        strcpy(request ,"GET /");
    }
    else if (strcmp(args[0] ,"client_post") == 0) {
        strcpy(cmd.request, POST_REQUEST);
        strcpy(request ,"POST /");
        strcpy(extra_data, (char *)read_from_file(("public/" + std::string(file_path))).c_str());
    }

    strcat(request,file_path); strcat(request," HTTP/1.1\n"); strcat(request,"Host: ");
    strcat(request,host_name); strcat(request,":"); strcat(request,port_number); strcat(request,"\n");

    if (strcmp(cmd.request ,POST_REQUEST) == 0) {
        strcat(request, CONTENT_LENGTH);
        int length_extra_data = std::string(extra_data).length();
        strcat(request, std::to_string(length_extra_data).c_str());
        strcat(request,ENDREQUEST); strcat(request,ENDREQUEST);
        strcat(request, extra_data);
    }
    strcat(request,ENDREQUEST);

    strcpy(cmd.file_path, file_path);
    strcpy(cmd.host_name, host_name);
    strcpy(cmd.port_number, port_number);
    strcpy(cmd.request_msg, request);

    return cmd;
}

char *send_recieve_routine(struct messege_content cmd, int sockfd) {

    int numbytes;

    printf("Sending %s request\n", cmd.request);

    if (send(sockfd, cmd.request_msg, std::string(cmd.request_msg).length(), 0) == -1) {
        printf("client: send error\n");
        exit(0);
    }

    printf("Sent\n");

    if ((numbytes = recv(sockfd, buffer, MAXDATASIZE-1, 0)) == -1) {
        printf("client: recv error\n");
        exit(1);
    }
    if (numbytes != 0) {
        buffer[numbytes] = '\0';
        printf("client: received from server on socket %d: %d '%s'\n", sockfd, numbytes, buffer);
        return buffer;
    }

    return NULL;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    // RUN AS : my_client hostname portNumber commands

	int sockfd; 
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char ipstr[INET6_ADDRSTRLEN];

    char PORT[] = "3490";
    char HOSTNAME[] = "localhost";
    char COMMANDFILE[] = "commands.txt";

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

    std::vector<char *> vec_of_commands;
    std::string cmds = read_from_file(COMMANDFILE);

    char cc[1000];strcpy(cc, cmds.c_str());
    char *token = strtok(cc, "\n"); 
    while (token != NULL) { 
        vec_of_commands.push_back(token);
        token = strtok(NULL, "\n"); 
    } 

    printf("Client will send\n");

    struct messege_content cmd[vec_of_commands.size()];
    for (int i=0;i<vec_of_commands.size();i++){
        printf("%s\n", vec_of_commands[i]);
        cmd[i] = request_preprocessing(vec_of_commands[i]);
    }

    printf("-------------------------------------------------------\n");

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(HOSTNAME, PORT, &hints, &servinfo)) != 0) {
		printf("getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			printf("client: create socket error\n");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			printf("client: connect error\n");
			close(sockfd);
			continue;
		}

        // convert the IP to a string and print it:
		inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)&p->ai_addr), ipstr, sizeof ipstr);
		printf("Client: successful connect at: %s\n", ipstr);

		break;
	}

	if (p == NULL) {
		printf("client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			ipstr, sizeof ipstr);
	printf("client: connecting to %s\n", ipstr);

	freeaddrinfo(servinfo); // all done with this structure

    // while(1)
    // {
        for (int i=0;i<vec_of_commands.size();i++)
        {
            if (!strcmp(cmd[i].request, GET_REQUEST)) {
                if (send_recieve_routine (cmd[i], sockfd) != NULL) {
                    save_recieved_content(buffer, cmd[i].file_path);
                    printf("-------------------------------------------------------------\n");
                }
            }
            else if (!strcmp(cmd[i].request, POST_REQUEST)) {
                send_recieve_routine (cmd[i], sockfd);
                printf("-------------------------------------------------------------\n");
            }
            usleep(2 * microsecond);//sleeps for 3 second
        }
        
    // }

	close(sockfd);

	return 0;
}
