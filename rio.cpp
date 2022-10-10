#include"rio.h"

ssize_t rio_readn(int fd, void * userbuf , ssize_t n){

    ssize_t nleft = n;//bytes unread
    size_t nread;
    char * bufp=(char *)userbuf;

    while(nleft>0){
        if((nread=read(fd,bufp,nleft))<0){
            if(errno==EINTR){ //interrupt,  call again
                nread=0;
            }
            else{
                return -1;
            }
        }
        else if (nread==0){ //eof
            break;
        }
        nleft-= nread;
        bufp+=nread;


    }
    return n-nleft;
}


ssize_t rio_writen(int fp, void * userbuf, ssize_t n){

    ssize_t nleft=n;
    char * bufp=(char * )userbuf;
    ssize_t nwrite;
    while(nleft>0){
        nwrite=write(fp,bufp,nleft);
        if(nwrite<=0){
            if(errno==EINTR){
                nwrite=0;
            }
            else{
                return -1;
            }            
        }
        nleft-=nwrite;
        bufp+=nwrite;
    }
    return n-nleft;

}



void rio_readinitb(rio_t * rp , const int fd){
    rp->rio_fd=fd;
    rp->rio_cnt=0;
    rp->rio_bufp=rp->rio_buf;
}


ssize_t rio_read( rio_t *rp ,char *usrbuf,size_t n){

    int cnt;
    while(rp->rio_cnt<=0){
        rp->rio_cnt=read(rp->rio_fd,rp->rio_buf,sizeof(rp->rio_buf));
        if (rp->rio_cnt==0){
            return 0;
        }
        else if(rp->rio_cnt<0){
            if(errno!=EINTR){
                return -1;
            }
        
        }
        else{
            rp->rio_bufp=rp->rio_buf;
        }

    }
    cnt=n;
    if(rp->rio_cnt<n){
        cnt=rp->rio_cnt;        
    }
    memcpy(usrbuf,rp->rio_bufp,cnt);
    rp->rio_bufp+=cnt;
    rp->rio_cnt-=cnt;
    return cnt;

}



ssize_t rio_readlineb(rio_t * rp, char * userbuf,size_t maxlen){

    int n,rc;
    char c;
    char *bufp=userbuf;
    for (n=1;n<=maxlen;n++){
        rc=rio_read(rp,&c,1);
        
        if(rc==0){
            if(n==1) return 0;
            else{
                break;
            }
        }
        else if (rc<0){
            return -1;
        }
        else{
            *bufp++=c;
            if(c=='\n'){
                n++;
                break;
            }
        }
    }
    *bufp=0;
    return n-1;
}

ssize_t rio_readnb(rio_t *rp,void *userbuf,size_t n){

    size_t nleft=n;
    ssize_t rn;
    char * bufp=(char *) userbuf;
    while(nleft>0){
        rn=rio_read(rp,bufp,n);
        if(rn==0) return 0;
        else if (rn<0){
            if (errno==EINTR) rn=0;
            else{
                return -1;
            }
        }
        nleft-=rn;
        bufp+=rn;
    }
    return n-nleft; //bytes has read
}


// #include<iostream>
// using namespace std;

// int main(){

//     char * buf=new char[100];

//     char wbuf[100]={"liuchengcai!"};
//     int fp=open("test.txt",O_RDONLY,0);
//     // ssize_t rn= rio_readn(fp,buf,30);
//     // printf("%s --- %d\n",buf,rn);
//     // close(fp);
//     // fp=open("test.txt",O_RDWR|O_APPEND);
//     // ssize_t wn=rio_writen(fp,wbuf,10);
//     // printf("----%d",wn);
//     rio_t rt;
//     rio_readinitb(&rt,fp);
    
//     rio_readnb(&rt,buf,30);
//     printf("%s  ---%d",buf,rt.rio_cnt);

//     char dest[236];
//     inet_pton(AF_INET,"192.168.3.4",dest);
//     cout<<dest<<endl;



// }