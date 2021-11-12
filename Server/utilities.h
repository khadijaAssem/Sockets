#pragma once
#ifndef UTILITIES_H
#define UTILITIES_H

#include "server.h"


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
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>    

// #define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold
#define MAXDATASIZE 1000000 // max number of bytes we can get at once 
#define MAXFILEPATHSIZE 50 // max number of bytes we can get at once 
#define MAXPORTNUMBSIZE 20 // max number of bytes we can get at once 
#define MAXHOSTNAMESIZE 50 // max number of bytes we can get at once 

#define GET_REQUEST "GET"
#define POST_REQUEST "POST"

#define OK_RESPONSE "HTTP/1.1 200 OK\r\n"
#define NOTFOUND_RESPONSE "HTTP/1.1 404 Not Found\r\n"
#define CONTENT_LENGTH "Content-Length: "
#define ENDREQUEST "\r\n"

#define TIMEOUT 5000000


struct messege_content {

    char request[8]; // GET or Post
    char file_path[MAXFILEPATHSIZE];
    char host_name[MAXHOSTNAMESIZE];
    char port_number[MAXPORTNUMBSIZE];
    char request_msg[MAXDATASIZE]; // Whole request messege

};

class utilities
{
    public:
    void save_data_to_path(char *, char *);
    std::string read_data_from_path(char *);
    struct messege_content request_processing(char[]);
    
};

#endif // UTILITIES_H