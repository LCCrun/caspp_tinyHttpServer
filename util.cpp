#include "rio.h"
#include"util.h"
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/mman.h>
#include<sys/wait.h>
#define MAXLINE 100
#define MAXBUF 300
void doit(int fd){
    //读取客户端发来的内容 判断方法 判断是静态还是动态
    //读是一行一行读  用rio_readlinedb
    char method[MAXLINE], url[MAXLINE], version[MAXLINE];
    char buf[MAXLINE],filename[MAXLINE],cgiargs[MAXLINE];
    struct stat sbuf;
    int is_static;
    rio_t rt;
    rio_readinitb(&rt,fd);
    rio_readlineb(&rt,buf,100);
    
    printf("\nrequest header : %s \n",buf);
    sscanf(buf,"%s %s %s",method, url, version);

    if(strcmp(method,"GET")){
        clienterror(fd,method,"501","Not implemented","Tiny does not implement this method");
        return;
    }
    read_requesthdrs(&rt);//读请求头

    is_static=parse_url(url,filename,cgiargs);

    if(stat(filename, &sbuf)<0){ //判断文件是否存在
        clienterror(fd,filename,"404","Not found","Tiny couldn't find this file");
        return ;
    }

    if(is_static){
        if(!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode) ){
            clienterror(fd,filename,"403","Forbidden","Tiny couldn't read the file");
            return ;
        }
        serve_static(fd,filename,sbuf.st_size);
    }
    else{

        if( !(S_ISREG(sbuf.st_mode))|| !(S_IXUSR & sbuf.st_mode) ){
            clienterror(fd,filename,"403","Foribidden","Tiny couldn't run the cgi program");

        }
        serve_dynamic(fd,filename,cgiargs);
    }


}


void read_requesthdrs(rio_t *rp){
    char buf[MAXLINE];

    rio_readlineb(rp,buf,MAXLINE);
    while(strcmp(buf,"\r\n")){
        rio_readlineb(rp,buf,MAXLINE);
        printf("%s",buf);

    }
    return ;

}



int parse_url(char * url,char *file,char *cgiargs){
    char * ptr;
    printf("------------url :%s\r\n",url);
    if(!strstr(url,"cgi-bin")){//static
        strcpy(cgiargs,"");
        strcpy(file,".");
        strcat(file,url);
        if(url[strlen(url-1)]=='/'){
            strcat(file,"home.html");
        }
        printf("---------static filename %s \r\n",file);
        return 1;
    }
    else{
        ptr=index(url,'?');
        if(ptr){
            strcpy(cgiargs,ptr+1);
            *ptr='\0';
        }
        else{
            strcpy(cgiargs,".");
        }
        strcpy(file,".");
        strcat(file,url);
        printf("---------dynamic filename %s \r\n",file);
        printf("---------cgiargv %s \r\n",cgiargs);
        return 0;
    }
}

void get_filetype(char *filename ,char *filetype){

    if(strstr(filename,".html")){
        strcpy(filetype,"text/html");

    }
    else if (strstr(filename,".gif")){
        strcpy(filetype,"image/gif");
    }
    else if (strstr(filename,".png")){
        strcpy(filetype,"image/png");
    }
    else if (strstr(filename,".jpg")){
        strcpy(filetype,"image/jpeg");    
    }
    else{
        strcpy(filetype,"text/plain");
    }


}

void serve_dynamic(int fd,char * filename,char *cgiargs){
    
    printf("---------serve dynamic-------");
    char buf[MAXBUF];
    char * emptylist[]={NULL};
    char * env[]={NULL};
    sprintf(buf,"HTTP/1.0 200 OK\r\n");
    rio_writen(fd,buf,strlen(buf));
    sprintf(buf,"Server: Tiny Web Server\r\n");
    rio_writen(fd,buf,strlen(buf));

    if(fork()==0){
        setenv("QUERY_STRING", cgiargs, 1);
        dup2(fd,STDOUT_FILENO);
        execl(filename,NULL);
    }

    wait(NULL);
    

}

void serve_static(int fd,char * filename, int filesize){
    printf("static html send......");
    int srcfd;
    char *srcp,filetype[MAXLINE],buf[MAXBUF];
    get_filetype(filename,filetype);

    sprintf(buf, "HTTP/1.0 200 OK\r\n" );
    sprintf(buf, "%sServer:Tiny Web Server\r\n", buf);
    sprintf(buf, "%sConnection: close\r\n", buf );
    sprintf(buf, "%sContent-lenght: %d\r\n",buf,filesize);
    sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
    rio_writen(fd, buf, strlen(buf));

    printf("\r\nRespose header\r\n%s",buf);


    srcfd=open(filename,O_RDONLY,0);
    if(srcfd<0) {
        printf("file read error : %s",strerror(errno));
        exit(1);
    }

    srcp=(char *)mmap(0,filesize,PROT_READ,MAP_PRIVATE,srcfd,0);

    close(srcfd);
    
    rio_writen(fd, srcp, filesize);

    munmap(srcp, filesize);

}

void clienterror(int fd,char *cause,char *errnum,char *shortmsg,char *longmsg){

    printf("server error %d %s \r\n",errnum,shortmsg);
    char buf[MAXLINE],body[MAXBUF];
    sprintf(body,"<html><title>Tiny Error</title>");
    sprintf(body,"%s<body bgcolor=""ffffff"" >\r\n",body);
    sprintf(body,"%s%s: %s\r\n",body,errnum,shortmsg);
    sprintf(body,"%s<p>%s:%s\r\n",body,longmsg,cause);
    sprintf(body,"%<hr><em> The tiny web server</em>\r\n",body);

    sprintf(buf,"HTTP/1.0 %s %s \r\n",errnum,shortmsg);
    rio_writen(fd,buf,strlen(buf));

    sprintf(buf,"Content-type: text/html\r\n");
    rio_writen(fd,buf,strlen(buf));
    sprintf(buf,"Content-length: %d\r\n\r\n",(int)strlen(body));
    rio_writen(fd,buf,strlen(buf));
    rio_writen(fd,body,strlen(body));


}