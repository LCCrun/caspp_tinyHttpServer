#ifndef RIO_H
#define RIO_H

#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<stdio.h>
#include<memory.h>
#include<arpa/inet.h>
#define RIO_BUFSIZE 8192
ssize_t rio_readn(int fd, void * userbuf , ssize_t n);


ssize_t rio_writen(int fp, void * userbuf, ssize_t n);


typedef struct 
{
    int rio_fd;
    int rio_cnt;
    char * rio_bufp;
    char rio_buf[RIO_BUFSIZE];

}rio_t;

void rio_readinitb(rio_t * rp , const int fd);

ssize_t rio_read( rio_t *rp ,char *usrbuf,size_t n);




ssize_t rio_readlineb(rio_t * rp, char * userbuf,size_t maxlen);


ssize_t rio_readnb(rio_t *rp,void *userbuf,size_t n);

#endif