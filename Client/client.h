#pragma once
#ifndef CLIENT_H
#define CLIENT_H

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

// #define PORT "3490" // the port client will be connecting to 

class client
{
    public:
    char *send_recieve_routine(struct messege_content cmd, int sockfd);
    void *get_in_addr(struct sockaddr *sa);
    int run(char HOSTNAME[], char PORT[], char COMMANDFILE[]);

};

#endif // CLIENT_H