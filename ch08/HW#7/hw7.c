#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void calculatePid(int i);

int main(int argc, char* argv[]) {
	int mypid = 1;
	int num = 0;

	printf("Enter the number of child processes: ");
	scanf(" %d", &num);
	printf("%d", num);

	for (int i = 1; i <= num && mypid != 0; i++) {
		if ((mypid = fork()) == -1) {
			perror("fork");
		}
		else if (mypid == 0) {
			printf("%d", mypid);
			calculatePid(i);
		}
		//else wait(NULL);
	}
	printf("All child processes completed.");
	return 0;
}

void calculatePid(int i) {
	int temp = getpid();
	printf("%d", temp);
	int remainder, result, oddoreven;

	if (temp % 2 == 0) {
		result = 1, oddoreven = 0;
		while (temp != 0) {
			remainder = temp % 10;
			if (remainder == 0)
				remainder = 1;
			result *= remainder;
		}
	}
	else {
		result = 0, oddoreven = 1;
		while(temp != 0) {
			remainder = temp % 10;
			result += remainder;
		}
	}
	printf("Child process %d [PID - %d]: result(%s) = %d\n", i, temp, oddoreven ? "odd" : "even", result);
	exit(17);
}