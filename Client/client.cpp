/*
** client.c -- a stream socket client demo
*/

#include "client.h"
#include "utilities.h"

unsigned int microsecond = 1000000;


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

char buffer[MAXDATASIZE];
utilities util;

char *client::send_recieve_routine(struct messege_content cmd, int sockfd) {

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
void *client::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int client::run(char HOSTNAME[], char PORT[], char COMMANDFILE[])
{
    // RUN AS : my_client hostname portNumber commands

	int sockfd; 
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char ipstr[INET6_ADDRSTRLEN];    

    std::vector<char *> vec_of_commands;
    std::string cmds = util.read_from_file(COMMANDFILE);

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
        cmd[i] = util.request_preprocessing(vec_of_commands[i]);
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
                    util.save_recieved_content(buffer, cmd[i].file_path);
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
