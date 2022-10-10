#include<sys/socket.h>
#include"util.h"
#include<stdlib.h>
#include"open_fd.h"
int main(int argc, char **argv){

    if(argc!=2){
        fprintf(stderr,"usage: %s <port>\n",argv[0]);
        exit(1);
    }

    char port[100];
    strcpy(port,argv[1]);
    int listenfd,connfd;
    listenfd = open_listenfd(port);
    sockaddr_storage client_addr;
    socklen_t addrlen=sizeof(client_addr);

    char client_hostname[100],client_port[100];


    while(1){
        connfd = accept(listenfd,(sockaddr * )&client_addr, &addrlen);
        if(connfd==-1){
            fprintf(stderr,"accept error: %s",strerror(errno));
            exit(1);
        }
        getnameinfo((sockaddr * )&client_addr, addrlen, client_hostname, 100, client_port, 100, 0);
        printf("connect to client (%s, %s) \r\n",client_hostname,client_port);

        doit(connfd);
        close(connfd);

    }



}