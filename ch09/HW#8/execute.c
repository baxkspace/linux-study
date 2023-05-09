#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>


int execute(char* argv[], int is_bg, int* pidch){
	int pid;
	int child_info = -1;
	
	if(argv[0] == NULL)
		return 0;
	if ((pid = fork()) == -1)
		perror("fork");
	else if (pid == 0){
		if (is_bg == 1) printf("PID : %d\n",*pidch = getpid());
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		execvp(argv[0], argv);
		perror("cannot execute command");
		exit(1);
	}else{
		if (is_bg == 0){
			if(wait(&child_info) == -1)
				perror("wait");
		}
	}
	return child_info;
}