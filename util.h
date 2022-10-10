
#include "rio.h"
void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_url(char * url,char *file,char *cgiargs);
void get_filetype(char *filename ,char *filetype);
void serve_dynamic(int fd,char * filename,char *cgiargs);
void serve_static(int fd,char * filename, int filesize);
void clienterror(int fd,char *cause,char *errnum,char *shortmsg,char *longmsg);

