/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

// #define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold
#define MAXDATASIZE 100 // max number of bytes we can get at once 

struct rcv_data 
{
	int sockfd;
	pthread_mutex_t tS_mutex;
	pthread_cond_t cond_trainFull, cond_trainArrived;
};


void *passenger_thread(void *arg)
{
	// struct rcv_data *rcv_data = (struct rcv_data*)arg;
    int numbytes;  
	char buf[MAXDATASIZE];
    int* new_fd = (int*)arg;
	printf("%d\n",*new_fd);
    if ((numbytes = recv(*new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
            printf("Server: recv error\n");
            exit(1);
	    }

    buf[numbytes] = '\0';
    printf("client: received '%s'\n",buf);
    close(*new_fd);
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

int main(int argc, char **argv)
{   
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

    // All ports below 1024 are RESERVED (unless you’re the superuser)! You can have any port number above that, right up to 65535
    char* PORT = "3490";

    if (argc >= 2){
        PORT = argv[1]; // get port number from user
    }

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
		int ret = pthread_create(&tid, NULL, passenger_thread, (void*)p);
	}

	return 0;
}
