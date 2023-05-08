#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int cnt = 0;

void f(int signum) {
	printf("Currently elapsed time: %d sec(s)\n", cnt);
}

int main() {
	void f(int);
	signal(SIGINT, f);
	printf("you can't stop me!\n");
	while (1) {
		sleep(1);
		cnt++;
		printf("haha\n");
	}
}