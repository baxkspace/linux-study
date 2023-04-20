#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFSIZ 4096
#define COPYMODE 0644

void oops(char *s1, char *s2);

int main(int ac, char *av[]) {
	int in_fd, out_fd, n_chars;
	char buf[BUFSIZ];
	char key;

	if (ac < 3) {
		fprintf(stderr, "usage: %s source destination\n", *av);
		exit(1);
	}

	if (ac == 3) { // default state
		if ((in_fd = open(av[1], O_RDONLY)) == -1) // input file open
			oops("Cannot open ", av[1]);
		if ((out_fd = creat(av[2], COPYMODE)) == -1)
			oops("Cannot creat ", av[2]);
	}
	
	// overwrite state
	if (ac == 4 && strcmp(av[1], "-I") == 0) {
		fprintf(stderr, "Overwrite the file? (y/n) : ");
		key = getchar();
		if (key == 'y') {
			if ((in_fd = open(av[2], O_RDONLY)) == -1) // input file open
				oops("Cannot open ", av[2]);
			out_fd = open(av[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
		}
		else if (key == 'n')
			exit(1);
		else fprintf(stderr, "wrong input.\n");
	}

	if (strcmp(av[1], "-P") == 0) { // permission
		struct stat info, info2;
		stat(av[2], &info);
		if ((in_fd = open(av[2], O_RDONLY)) == -1) // input file open
			oops("Cannot open ", av[2]);

		mode_t mode = info.st_mode;
		printf("%d\n", mode);

		out_fd = creat(av[3], COPYMODE);
		stat(av[3], &info2);
		info2.st_mode = mode;
	}

	if (strcmp(av[1], "-IP") == 0 || strcmp(av[1], "-PI")) {
		struct stat info;
		printf(stderr, "Overwrite the file? (y/n) : ");
		key = getchar();
		if (key == 'y') {
			if ((in_fd = open(av[2], O_RDONLY)) == -1) // input file open
				oops("Cannot open ", av[2]);
			mode_t mode = info.st_mode;
			out_fd = open(av[3], mode);
		}
		else if (key == 'n')
			exit(1);
		else fprintf(stderr, "wrong input.\n");
	}

	while ((n_chars = read(in_fd, buf, BUFSIZ)) > 0)
		if (write(out_fd, buf, n_chars) != n_chars)
			oops("write error to ", av[2]);

		if (n_chars == -1)
			oops("read error from ", av[1]);
		if (close(in_fd) == -1 || close(out_fd) == -1)
			oops("error closing file ", "");
}

void oops(char *s1, char *s2) {
	fprintf(stderr, "error: %s", s1);
	perror(s2);
	exit(1);
}