#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>

int cnt = 0;
int fullcount = 0;

void repeat(int);

void f(int signum) {
	printf("\n=============================================\n");
	printf("Total number of printed HelloWorld!: %d\n", cnt);
	printf("=============================================\n");
	if (cnt % 2 == 1)
		exit(1);
}

void f2(int signum) {
	printf("\n=============================================\n");
	printf("Total number of printed HelloWorld!: %d\n", cnt);
	printf("Number of HelloWorld! prints remaining: %d\n", fullcount - cnt);
	printf("=============================================\n");
	if (cnt % 2 == 1)
		exit(1);
}

int main(int argc, char* argv[]) {
	void f(int);
	void f2(int);

	if (argc == 1) {
		signal(SIGINT, f);
		repeat(fullcount);
	}
	else if (argc == 2) {
		printf("Please specify the limit of HelloWorld! to print: ");
		scanf("%d", &fullcount);
		signal(SIGINT, f2);
		repeat(fullcount);
	}
	else {
		if (atoi(argv[2]) < 0)
			printf("Invalid input. Please enter a positive integer.\n");
		else {
			fullcount = atoi(argv[2]);
			signal(SIGINT, f2);
			repeat(fullcount);
		}
	}
}

void repeat(int fullcount) {
	if (fullcount == 0) {
		while (1) {
			sleep(1);
			printf("HelloWorld!\n");
			cnt++;
		}
	}
	else {
		while (fullcount--) {
			sleep(1);
			printf("HelloWorld!\n");
			cnt++;
			if (fullcount == 0) {
				printf("=============================================\n");
				printf("Total number of printed HelloWorld!: %d\n", cnt);
				printf("Number of HelloWorld! prints remaining: %d\n", 0);
				printf("=============================================\n");
			}
		}
	}
}