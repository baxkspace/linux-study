/* 
	#1: when row/col of the ball is bigger than boundaries... 
		it breaks walls
	#2: slow down coordinate of the ball
*/

#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

#define BLANK " "

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
	
	if (row > endrow)
		row = endrow;
	else if (row < startrow)
		row = startrow;
	else if (col > endcol)
		col = endcol;
	else if (col < startcol)
		col = startcol;
	move(row, col);
		
	if (row <= endrow && row >= startrow && col <= endcol && col >= startcol)
	{	
		addstr(ball);
		move(LINES - 1, 0);
	}
	refresh();

	if (row >= endrow || row <= startrow) {
		ndelay = delay * 2;
		row_dir = -row_dir;
		set_ticker(ndelay);
		delay = ndelay;
	}
	if (col >= endcol || col <= startcol) {
		ndelay = delay / 2;
		col_dir = -col_dir;
		set_ticker(ndelay);
		delay = ndelay;
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

	initscr();
	crmode();
	noecho();
	clear();

	draw_bound(startrow - 1, endrow + 1, startcol - 1, endcol + 1);

	row = atoi(argv[2]);
	col = atoi(argv[3]);
	move(row, col);
	addstr(ball);
	
	signal(SIGALRM, move_ball);
	set_ticker(delay);
	
	while(true) {
		ndelay = 0;
		c = getch();
		if (c == 'Q') break;
		if (c == 'q') {
			if (col_dir > 1)
				col_dir /= 2;
		}
		if (c == 'w') {
			if (row_dir > 1)
				row_dir /= 2;
		}
		if (c == 'e') col_dir *= 2;
		if (c == 'r') row_dir *= 2;
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
