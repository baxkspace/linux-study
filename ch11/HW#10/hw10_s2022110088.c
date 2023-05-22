#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

#define BUF_SIZE 100

FILE* popen2(const char*, const char*);
void myhandler(int sig);

void myhandler(int sig) {
	signal(sig, SIG_IGN);
	if (sig == SIGINT) {
		fprintf(stdout, "     SIGINT received but ignored...\n");
	}
	else if (sig == SIGQUIT) {
		fprintf(stdout, "     SIGQUIT received but ignored...\n");
	}
	else if (sig == SIGTERM) {
		fprintf(stdout, "     SIGTERM received but ignored...\n");
	}
}

int main(void) {
	FILE *pipein_fp, *pipeout_fp;
	char readbuf[BUF_SIZE];

	signal(SIGINT, myhandler);
	signal(SIGQUIT, myhandler);
	signal(SIGTERM, myhandler);

	if((pipein_fp = popen2("ls", "r")) == NULL) {
		perror("peopen2");
		exit(1);
	}

	if ((pipeout_fp = popen2("sort -r", "w")) == NULL) {
		perror("popen2");
		exit(1);
	}

	printf("PID: %d\n", getpid());
	for (int i = 0; i < 3; i++)
		sleep(1);

	while (fgets(readbuf, BUF_SIZE, pipein_fp))
		fputs(readbuf, pipeout_fp);

	pclose(pipeout_fp);
	pclose(pipein_fp);

	for (int i = 0; i < 10; i++)
		sleep(1);

	return 0;
}

FILE *popen2(const char* command, const char *mode) {
	void myhandler(int sig);

	int pipe_list[2], Pend, Cend;
	pid_t pid;
	FILE* fp;

	if (strcmp(mode, "r") == 0) {
		Pend = 0; Cend = 1;
	}
	if (strcmp(mode, "w") == 0) {
		Pend = 1; Cend = 0;
	}
	if (pipe(pipe_list) == -1) {
		perror("pipe");
		exit(1);
	}

	pid = fork();

	if (pid > 0) {
		signal(SIGINT, myhandler);
		signal(SIGQUIT, myhandler);
		signal(SIGTERM, myhandler);

		close(pipe_list[Cend]);
		if (Pend == 0)
			fp = fdopen(pipe_list[Pend], "r");
		if (Pend == 1)
			fp = fdopen(pipe_list[Pend], "w");
		return fp;
	}
	else if (pid == 0) {
		signal(SIGINT, myhandler);
		signal(SIGQUIT, myhandler);
		signal(SIGTERM, myhandler);

		close(pipe_list[Pend]);
		dup2(pipe_list[Cend], Cend);
		close(pipe_list[Cend]);

		execl("/bin/sh", "sh", "-c", command, NULL);
		exit(1);
	}
	else exit(1);
}