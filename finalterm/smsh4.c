#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "smsh.h"
#include "varlib.h"

#define DFL_PROMPT "> "

int main(int argc, char* argv[]) {
	char *cmdline, *prompt, **arglist;
	int result, process(char**);
	void setup(int);

	int setnum = 0;
	prompt = DFL_PROMPT;
	setup(0);
	while ((cmdline = next_cmd(prompt, stdin)) != NULL) {
		if ((arglist = splitline(cmdline)) != NULL) {
			result = process(arglist);
			if (result == -1) {
				freelist(arglist);
				free(cmdline);
				setup(1);
				break;
			}
			freelist(arglist);
		}
		free(cmdline);
	}
	return 0;
}

void fatal(char* s1, char* s2, int n) {
	fprintf(stderr, "Error: %s, %s\n", s1, s2);
	exit(n);
}

void setup(int num) {
	int pid;
	extern char** environ;
	VLenviron2table(environ);
	signal(SIGALRM, SIG_IGN);
	signal(SIGKILL, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	if (num == 0) {
		if ((pid = fork())!= -1) {
			if (pid > 0) {
				while (1) {
					alarm(0);
				}
			}
		}
	}
	else if (num == 1) {
		if ((pid = getpid())> 0)
			exit(1);
		if ((pid = getpid())<= 0)
			exit(1);
	}
}