#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "smsh.h"

#define DFL_PROMPT "> "

int main() {
	int pid, background = 0;
	char *cmdline, *prompt, **arglist;
	int result;
	void setup();

	prompt = DFL_PROMPT;
	setup();
	while ((cmdline = next_cmd(prompt, stdin, &background)) != NULL) {
		if ((arglist = splitline(cmdline)) != NULL) {
			if (strcmp(arglist[0], "exit") == 0) {
				kill(pid, SIGINT);
				exit(0);
			}
			else {
				result = execute(arglist, background, &pid);
				freelist(arglist);
			}
		}
		background = 0;
		free(cmdline);
	}
	return 0;
}

void setup() {
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void fatal(char *s1, char *s2, int n) {
	fprintf(stderr, "Error: %s, %s\n", s1, s2);
	exit(n);
}