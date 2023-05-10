#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

void main() {
	int pid, fd;
	printf("About to run ps into a file\n");
	if ((pid = fork()) == -1) {
		perror("fork");
		exit(1);
	}
	if (pid == 0) {
		close(1);
		fd = creat("psfile", 0644);
		execlp("ps", "ps", "aux", NULL);
		perror("execlp"); exit(1);
	}
	if (pid != 0) {
		wait(NULL);
		printf("Done running ps, results in psfile\n");
	}
	return;
}