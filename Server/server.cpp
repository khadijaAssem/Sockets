/*
** server.c -- a stream socket server demo
*/
#include "server.h"
/*  GET /file-path HTTP/1.1
	Host: host-name:(port-number)  */


utilities utils;

void *thread_handler(void *arg)
{
    int numbytes;  
    char buf[MAXDATASIZE];
    int* new_fd = (int*)arg;

	auto start = std::chrono::system_clock::now(); // Restart timer

    while((std::chrono::system_clock::now() - start).count() < TIMEOUT ){

        if ((numbytes = recv(*new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
            printf("Server: recv error\n");
            break;
        }

        if (numbytes != 0) {

			start = std::chrono::system_clock::now(); // Restart timer

            buf[numbytes] = '\0';
			struct messege_content cmd = utils.request_processing(buf);

            printf("Server: received from client on socket %d: '%s request'\n%s\n", *new_fd, cmd.request, cmd.request_msg);

			if (!strcmp(cmd.request, GET_REQUEST)) {
				std::string fileTXT = utils.read_data_from_path(cmd.file_path);
				std::string response = "";
				if (fileTXT.length() > 0) {
					// SEND OK RESPONSE AND CONCAT
					response += OK_RESPONSE;
					response += (CONTENT_LENGTH + std::to_string(fileTXT.length()) + "\n");
					response += ENDREQUEST;response += fileTXT;response += ENDREQUEST;
				}
				else {
					// SEND NOT FOUND
					response += NOTFOUND_RESPONSE;
				}

				// Actual SEND
				if (send(*new_fd, response.c_str(), response.length(), 0) == -1) {
					printf("Server: send error\n");
					break;
            	}
			}
			
			else if (!strcmp(cmd.request, POST_REQUEST)) {
				utils.save_data_to_path(cmd.request_msg, cmd.file_path);

				if (send(*new_fd, OK_RESPONSE, std::string(OK_RESPONSE).length(), 0) == -1) {
					printf("Server: send error\n");
					break;
            	}
			}
			printf("------------------------------------------------------------------------\n");
        }
    }
	printf("CLOSING CONNECTION WITH CLIENT SINCE TIMEOUT\n");
    close(*new_fd);
	return NULL;
}

// get sockaddr, IPv4 or IPv6:
void *server::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int server::run(char PORT[])
{   
	// RUN AS : my_server portNumber

	int sockfd, new_fd;  // listen on sock_fd (descriptor), new connection on new_fd
	struct addrinfo hints, *servinfo, *p; // servinfo Where getaddrInfo output will be stored
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char ipstr[INET6_ADDRSTRLEN];
    int numbytes;  
	char buf[MAXDATASIZE];
	int rv;

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever (AF_INET or AF_INET6)
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE; // assign the address of my local host to the socket structures
    // lpcalhost was null but I used local host instead
	if ((rv = getaddrinfo("localhost", PORT, &hints, &servinfo)) != 0) { // Returns non zero if error
        printf("getaddrinfo: %s\n", gai_strerror(rv));
		printf("getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			printf("server: create socket error\n");
			continue;
		}

        /* Sometimes, you might notice, you try to rerun a server and bind() 
        fails, claiming “Address already in use.” What does that mean? Well, 
        a little bit of a socket that was connected is still hanging around in the kernel, 
        and it’s hogging the port. 
        You can either wait for it to clear (a minute or so), 
        or add code to your program allowing it to reuse the port, like this */

        // lose the pesky "Address already in use" error message
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			printf("server: setsockopt error\n");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			printf("server: bind error\n");
			continue;
		}
        
		// convert the IP to a string and print it:
		inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)&p->ai_addr), ipstr, sizeof ipstr);
		printf("Successful bind at: %s\n", ipstr);

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

    // reached the end of the linkedlist and didn't break so never binded
	if (p == NULL)  {
		printf("server: failed to bind\n");
		exit(1);
	}

    // incoming connections are going to wait in a queue until you accept() them.
    // backlog is the number of connections allowed on the incoming queue.
	if (listen(sockfd, BACKLOG) == -1) {
		printf("server: listen\n");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;

        /* You call accept() to get the pending connection. 
        It’ll return to you a brand new socket file descriptor to use for this single connection! 
        So you have two socket file descriptors for the price of one! 
        The original one is still listening for more new connections, 
        and the newly created one is finally ready to send() and recv().*/

        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

        if (new_fd == -1) {
            printf("Server: accept error\n");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            ipstr, sizeof ipstr);
        
        printf("server: got connection from %s\n", ipstr);

        // ready to communicate on socket descriptor new_fd!
        // Will use send() for sending files and HTTP responses and recv() for recieving the request

        pthread_t tid;
        int* p = (int*)malloc(sizeof(int));
        *p = new_fd;
		int ret = pthread_create(&tid, NULL, thread_handler, (void*)p);
	}

	return 0;
}
