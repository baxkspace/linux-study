#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

void read_til_crnl(FILE*);
void process_rq(char*, int);
void header(FILE*, char*);
void cannot_do(int);
void do_404(char*, int);
int isadir(char*);
int not_exist(char*);
char* file_type(char*);
int ends_in_cgi(char*);

void do_ls(char*, int);
void do_exec(char*, int);
void do_cat(char*, int);