#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define BUF_SIZ 1024

int fileDelete(char* filename) {
	int fileLink = unlink(filename);
	if (fileLink != 0)
		return 1;
	else return 0;
}

int main(int argc, char *argv[]) {
	char buffer[BUF_SIZ];
	FILE* 
}