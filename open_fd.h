#ifndef OPENFD_H
#define OPENFD_H

#include<netdb.h>
#include<sys/types.h>
#include<stdio.h>
#include<memory.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/socket.h>
#define LISTENQ 100000
int open_clentfd(char * hostname,char * port);

int open_listenfd(char * port);

#endif