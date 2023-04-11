/* 
	#1: have to add features - slow down/speed up coordinate of the ball
	#2: fix errors - if ball touches boundaries, speed of the ball should be double/half
	so how? tlqkf...
*/

#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

#define BLANK "       "

int delay = 500, ndelay = 0;
char* ball;
int row, col;
int startrow, endrow, startcol, endcol;
int row_dir, col_dir;

void draw_bound(int startrow, int startcol, int endrow, int endcol);

int set_ticker(int n_msecs) {
	struct itimerval new_timeset;
	long n_sec, n_usecs;

	n_sec = n_msecs / 1000;
	n_usecs = (n_msecs % 1000) * 1000L;

	new_timeset.it_interval.tv_sec = n_sec;
	new_timeset.it_interval.tv_usec = n_usecs;
	new_timeset.it_value.tv_sec = n_sec;
	new_timeset.it_value.tv_usec = n_usecs;

	return setitimer(ITIMER_REAL, &new_timeset, NULL);
}

void move_ball(int signum) {
	signal(SIGALRM, move_ball);
	move(row, col);
	addstr(BLANK);
	col += col_dir;
	row += row_dir;
	move(row, col);
	addstr(ball);
	refresh();

	if (row == endrow || row == startrow) {
		ndelay = delay * 2;
		row_dir = -row_dir;
		refresh();
		}
	if (col == endcol || col == startcol) {
		ndelay = delay / 2;
		col_dir = -col_dir;
		refresh();
	}
}

int main(int argc, char* argv[]) {
	int c;
	ball = argv[1];
	row_dir = 1;
	col_dir = 1;

	startrow = atoi(argv[4]);
	endrow = atoi(argv[5]);
	startcol = atoi(argv[6]);
	endcol = atoi(argv[7]);
	void move_msg(int);

	crmode();
	noecho();
	clear();

	
	initscr();
	clear();
	draw_bound(startrow - 1, endrow + 1, startcol - 1, endcol + 1);
	//refresh();

	row = atoi(argv[2]);
	col = atoi(argv[3]);
	move(row, col);
	addstr(ball);
	
	signal(SIGALRM, move_ball);
	set_ticker(delay);
	
	while(true) {
		//ndelay = 0;
		c = getch();
		if (c == 'Q') break;
		if (c == 'q') col_dir /= 2;
		if (c == 'w') row_dir /= 2;
		if (c == 'e') col_dir *= 2;
		if (c == 'r') row_dir *= 2;
		if (ndelay > 0) {
			set_ticker(ndelay);
			delay = ndelay;
		}
	}
	endwin();
	return 0; 
}

void draw_bound(int startrow, int endrow, int startcol, int endcol) {
	move(startrow, startcol);
	for (int i = 0; i <= endcol - startcol; i++) {
		addstr("*");
	}

	for (int i = 0; i <= endrow - startrow; i++) {
		move(startrow + i, startcol);
		addstr("*");
		move(startrow + i, endcol);
		addstr("*");
	}

	move(endrow, startcol);
	for (int i = 0; i <= endcol - startcol; i++) {
		addstr("*");
	}
}
