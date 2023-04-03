#define _UNIX03_WITHDRAWN
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

struct flaginfo {
	int fl_value;
	char *fl_name_lower;
	char* fl_name_upper;
};

void showbaud(int thespeed);
void show_some_flags(struct termios *ttyp);
void show_flagset(int thevalue, struct flaginfo thebitnames[]);
int flag_bit(struct flaginfo thebitnames[], char* flagname);
void printspeed();

struct flaginfo input_flags[] = {
	{IGNBRK, "ignbrk", "IGNBRK"},
	{BRKINT, "brkint", "BRKINT"},
	{IGNPAR, "ignpar", "IGNPAR"},
	{PARMRK, "parmrk", "PARMRK"},
	{INPCK, "inpck", "INPCK"},
	{ISTRIP, "istrip", "ISTRIP"},
	{INLCR, "inlcr", "INLCR"},
	{IGNCR, "igncr", "IGNCR"},
	{ICRNL, "icrnl", "ICRNL"},
	{IXON, "ixon", "IXON"},
	{IXOFF, "ixoff", "IXOFF"},
	{0, "null", "NULL"}
};

struct flaginfo local_flags[] = {
	ISIG, "isig", "ISIG",
	ICANON, "icanon", "ICANON",
	ECHO, "echo", "ECHO",
	ECHOE, "echoe", "ECHOE",
	ECHOK, "echok", "ECHOK",
	0, "null", "NULL"
};

int output_flags[] = {
	OFILL, OCRNL, OLCUC, ONLCR, ONLRET, ONOCR, OPOST, OFDEL
};

int main(int argc, char *argv[]) {
	struct termios ttyinfo;
	if (tcgetattr(0, &ttyinfo) == -1) {
		perror("cannot get params about stdin");
		exit(1);
	}
		
	
	if (argc == 1) {
		printspeed(&ttyinfo);
		show_some_flags(&ttyinfo);
	}
	else if (strcmp(argv[1], "-FI") == 0) {
		printspeed(&ttyinfo);
		for (int cnt = 2; cnt < argc; cnt++)
		{
			int flagnum = flag_bit(input_flags, argv[cnt]);
			int flagbit = input_flags[flagnum].fl_value;

			if (ttyinfo.c_iflag & flagbit)
				ttyinfo.c_iflag &= ~flagbit;
			else ttyinfo.c_iflag |= flagbit;
		}
		tcsetattr(0, TCSANOW, &ttyinfo);
		show_some_flags(&ttyinfo);
	}
	else if (strcmp(argv[1], "-FO") == 0) {
		if (ttyinfo.c_oflag & OLCUC)
			ttyinfo.c_oflag &= ~OLCUC;
		else
			ttyinfo.c_oflag |= OLCUC;

		/*if (ttyinfo.c_oflag & OPOST)
			ttyinfo.c_oflag &= ~OPOST;
		else
			ttyinfo.c_oflag |= OPOST;*/

		tcsetattr(0, TCSANOW, &ttyinfo);
		printspeed(&ttyinfo);
		show_some_flags(&ttyinfo);
	}
	else if (strcmp(argv[1], "-FL") == 0) {
		printspeed(&ttyinfo);
		for (int cnt = 2; cnt < argc; cnt++)
		{
			int flagnum = flag_bit(local_flags, argv[cnt]);
			int flagbit = local_flags[flagnum].fl_value;

			if (ttyinfo.c_iflag & flagbit)
				ttyinfo.c_iflag &= ~flagbit;
			else ttyinfo.c_iflag |= flagbit;
		}
		tcsetattr(0, TCSANOW, &ttyinfo);
		show_some_flags(&ttyinfo);
	}
	else if (strcmp(argv[1], "-A") == 0) {
		for (int i = 0; i < 12; i++) {
			if (ttyinfo.c_iflag & input_flags[i].fl_value)
				ttyinfo.c_iflag &= ~input_flags[i].fl_value;
			else ttyinfo.c_iflag |= input_flags[i].fl_value;

			if (ttyinfo.c_lflag & local_flags[i].fl_value)
				ttyinfo.c_lflag &= ~local_flags[i].fl_value;
			else ttyinfo.c_lflag |= local_flags[i].fl_value;

			if (ttyinfo.c_oflag & output_flags[i])
				ttyinfo.c_oflag &= ~output_flags[i];
			else ttyinfo.c_oflag |= output_flags[i];
		}
		printspeed(&ttyinfo);
		tcsetattr(0, TCSANOW, &ttyinfo);
		show_some_flags(&ttyinfo);
	}
}

void showbaud(int thespeed) {
	printf("speed ");
	switch (thespeed) {
		case B300: printf("300 baud;\n"); break;
		case B600: printf("600 baud;\n"); break;
		case B1200: printf("1200 baud;\n"); break;
		case B1800: printf("1800 baud;\n"); break;
		case B2400: printf("2400 baud;\n"); break;
		case B4800: printf("4800 baud;\n"); break;
		case B9600: printf("9600 baud;\n"); break;
		default: printf("is fast;\n"); break;
	}
}

void show_some_flags(struct termios *ttyp) {
	show_flagset(ttyp->c_iflag, input_flags);
	show_flagset(ttyp->c_iflag, local_flags);
}

int flag_bit(struct flaginfo thebitnames[], char* flagname) {
	for (int i = 0; i < 12; i++) {
		//printf("%s %s", flagname, thebitnames[i].fl_name_lower);
		if (strcmp(flagname, thebitnames[i].fl_name_lower) == 0 )
			return i;
	}
	return -1;
}

void show_flagset(int thevalue, struct flaginfo thebitnames[]) {
	for (int i = 0; thebitnames[i].fl_value; i++) {
		if (thevalue & thebitnames[i].fl_value)
			printf("");
		else printf("-");
		printf("%s ", thebitnames[i].fl_name_lower);
	}
	puts("");
}

void printspeed(struct termios *ttyp) {
	showbaud(cfgetospeed(&ttyp));
	printf("erase = ^%c; kill = ^%c;\n", ttyp->c_cc[VERASE]-1+'A', ttyp->c_cc[VKILL]-1+'A');
}