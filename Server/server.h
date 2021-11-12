#pragma once
#ifndef SERVER_H
#define SERVER_H

#include "utilities.h"

class server 
{
    public:
    void *get_in_addr(struct sockaddr *sa);
    int run(char PORT[]);
};

#endif // SERVER_H