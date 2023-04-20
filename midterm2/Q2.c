#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

void wrongchar(int signum) {
	printf("illegal number entered! \nSIGUSR1 signal accept! Exit program.\n");
	exit(1);
}

void wrongdominate(int signum) {
	printf("Denominator is 0!!\n");
	printf("SIGFPE signal accept! Exit program.\n");
	exit(2);
}

int main(void) {
	char operator;
	char num1[100], num2[100];
	float result;
	char ch = 'Y';

	int pid = getpid();

	signal(SIGINT, SIG_IGN);
	signal(SIGUSR1, wrongchar);
	signal(SIGFPE, wrongdominate);
	
	do {
		printf("Enter the first number: "); 
		scanf("%s", num1);
		getchar();
		printf("Enter an operator (+, -, *, /): ");
		scanf(" %c", &operator);
		printf("Enter the second number: ");
		scanf("%s", num2);

		if (isalpha(num2[0]) != 0) {
			kill(pid, SIGUSR1);
		}

		float num11, num22;
		num11 = (float)atoi(num1);
		num22 = (float)atoi(num2);

		if (operator == '/' && num22 == 0) {
			kill(pid, SIGFPE);
		}
		switch(operator){
		case '+': result = num11 + num22;
			break;
		case '-': result = num11 - num22;
			break;
		case '*': result = num11 * num22;
			break;
		case '/': result = num11 / num22;
			break;
		}
		printf("%.2f %c %.2f = %.2f\n", num11, operator, num22, result);

		printf("Would you like to calculate again? (Y/N): ");
		ch = getchar();
	} while (ch == 'N');
	
}