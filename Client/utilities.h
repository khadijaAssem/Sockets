#pragma once
#ifndef UTILITIES_H
#define UTILITIES_H

#include "client.h"

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
    std::string read_from_file (std::string path);
    void save_recieved_content(char *buffer, char *file_path);
    struct messege_content request_preprocessing(char command[]);
};

#endif // UTILITIES_H