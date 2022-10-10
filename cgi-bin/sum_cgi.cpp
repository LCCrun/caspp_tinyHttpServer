#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#define MAXLINE 100
int main(){

    int n1=0,n2=0;
    char arg1[MAXLINE],arg2[MAXLINE],content[MAXLINE];
    char *buf,*p;

    if((buf = getenv("QUERY_STRING"))!=nullptr){
        
        p=strchr(buf,'&');
        *p='\0';
        strcpy(arg1,buf);
        strcpy(arg2,p+1);
        n1 = atoi(arg1);
        n2 = atoi(arg2);
    }

    sprintf(content,"wlecome to add.com\r\n<p>");
    sprintf(content,"%s the answer is %d + %d = %d\r\n<p>",content,n1,n2,n1+n2);
    sprintf(content,"%s thank you for visiting!\r\n",content);


    //生成http相应行和相应头

    printf("Connection: close\r\n");
    printf("Content-lenght: %d\r\n",(int)strlen(content));
    printf("content-type: text/html \r\n\r\n");
    printf("%s",content);
    fflush(stdout);

    return 0;

}