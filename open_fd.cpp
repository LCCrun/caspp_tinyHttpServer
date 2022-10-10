#include"open_fd.h"
#include<unistd.h>
int open_clentfd(char * hostname,char * port){

    int clientfd;
    addrinfo hints ,*listp,*p;
    memset(&hints,0,sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;
    hints.ai_flags |= AI_ADDRCONFIG;
    getaddrinfo(hostname, port, &hints,&listp);

    for(p=listp; p; p=p->ai_next){
        if((clientfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol))<0)
            continue;
        
        if(connect(clientfd,p->ai_addr,p->ai_addrlen)!=-1)
            break;
        close(clientfd);
    }

    freeaddrinfo(listp);
    if(!p) return -1;
    else{
        return clientfd;
    }

}

int open_listenfd(char * port){

    int listenfd;
    int optval=1;
    addrinfo hints,*listp,*p;
    memset(&hints,0,sizeof(addrinfo));
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_NUMERICSERV|AI_PASSIVE;
    hints.ai_flags|=AI_ADDRCONFIG;
    getaddrinfo(nullptr,port,&hints,&listp);
    
    for(p=listp; p; p=p->ai_next){
        listenfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol);
        if(listenfd<0) {
            continue;
        }
        setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const void *)optval,sizeof(int));

        if(bind(listenfd,p->ai_addr,p->ai_addrlen)==0) {
            printf("bind ok !\n");
            break;
        }
        close(listenfd);
    }

    freeaddrinfo(listp);

    if(!p) return -1;

    if(listen(listenfd,LISTENQ)<0){
        close(listenfd);
        return -1;
    }
    printf("listenfd set ok !\n");
    return listenfd;

}

