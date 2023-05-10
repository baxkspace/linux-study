#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define CLOSE_DUP
#define MAXLINE 100
const char* TARGET_FILE = "/etc/passwd";

void main() {
	int fd, newfd;
	char line[MAXLINE];

	fgets(line, MAXLINE, stdin);
	printf("%s", line);
	fgets(line, MAXLINE, stdin);
	printf("%s", line);
	fgets(line, MAXLINE, stdin);
	printf("%s", line);

	fd = open(TARGET_FILE, O_RDONLY);

	#ifdef CLOSE_DUP
		close(0);
		newfd = dup(fd);
	#else
		newfd = dup2(fd, 0);
	#endif
		if (newfd != 0) {
			fprintf(stderr, "Couldn't duplicate fd to 0\n");
			exit(1);
		}
		close(fd);
		fgets(line, MAXLINE, stdin);
		printf("%s", line);
		fgets(line, MAXLINE, stdin);
		printf("%s", line);
		fgets(line, MAXLINE, stdin);
		printf("%s", line);
}