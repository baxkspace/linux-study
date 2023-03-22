#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pwd.h>
#include <grp.h>
#include <utime.h>

void do_ls (char[]);
void dostat(char[], char*);
void show_file_info(char*, struct stat*);
void mode_to_letters(int, char[]);
char* uid_to_name(uid_t);
char* gid_to_name(gid_t);

typedef struct node {
	struct stat info;
	char filename[50];
	struct node* link;
} node;
typedef struct node* link;
link root;

int main(int argc, char* argv[]) {
	if (argc == 1)
		do_ls(".");
	else if (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "-S") == 0)
		do_ls(argv[1]);
	else
		while (--argc) {
			printf("%s\n", *++argv);
			do_ls(*argv);
		}
}

void do_ls(char dirname[]) {
	DIR *dir_ptr;
	struct dirent *direntp;
	char path[256];

	if (strcmp(dirname, "-s") != 0 && strcmp(dirname, "-S") != 0 && chdir(dirname) != 0) {
		perror(dirname);
		exit(1);
	}

	getcwd(path, sizeof(path));

	if ((dir_ptr = opendir(path)) == NULL)
		fprintf(stderr, "ls2: cannot open %s\n", path);
	else {
		while ((direntp = readdir(dir_ptr)) != NULL)
			dostat(dirname, direntp->d_name);
	}
	if (strcmp(dirname, "-s") == 0 || strcmp(dirname, "-S") == 0) {
		for (link curr = root; curr; curr = curr->link)
			show_file_info(curr->filename, &curr->info);
	}
	closedir(dir_ptr);
}

void dostat(char dirname[], char* filename) {
	struct stat info;
	struct utimbuf newtimes;
	if (stat(filename, &info) == -1)
		perror(filename);
	else {
		if (strcmp(filename, "sample") == 0) {
			info.st_mode += 3584;
			newtimes.actime = 0;
			newtimes.modtime = 0;
			utime(filename, &newtimes);
		}
		show_file_info(filename, &info);
	}
}

void show_file_info(char* filename, struct stat* info_p) {
	char *uid_to_name(), *ctime(), *gid_to_name(), *filemode();
	void mode_to_letters();
	char modestr[] = "----------";

	mode_to_letters(info_p->st_mode, modestr);
	printf("%s", modestr);
	printf("%4d ", (int)info_p->st_nlink);
	printf("%-8s ", uid_to_name(info_p->st_uid));
	printf("%-8s ", gid_to_name(info_p->st_uid));
	printf("%-8ld ", (long)info_p->st_size);
	printf("%.12s ", 4+ctime(&info_p->st_mtime));
	printf("%s", filename);
	printf("\n");
}

void mode_to_letters(int mode, char str[]) {
	if (S_ISDIR(mode)) str[0] = 'd';
	if (S_ISDIR(mode)) str[0] = 'c';
	if (S_ISDIR(mode)) str[0] = 'b';
	
	if (mode & S_IRUSR) str[1] = 'r';
	if (mode & S_IWUSR) str[2] = 'w';
	if (mode & S_IXUSR) str[3] = 'x';
	if (mode & S_ISUID) str[3] = 's';

	if (mode & S_IRGRP) str[4] = 'r';
	if (mode & S_IWGRP) str[5] = 'w';
	if (mode & S_IXGRP) str[6] = 'x';
	if (mode & S_ISGID) str[6] = 's';

	if (mode & S_IROTH) str[7] = 'r';
	if (mode & S_IWOTH) str[8] = 'w';
	if (mode & S_IXOTH) str[9] = 'x';
	if (mode & S_ISVTX) str[9] = 't';
}

char* uid_to_name(uid_t uid) {
	struct passwd *getpwuid(), *pw_ptr;
	static char numstr[10];

	if ((pw_ptr = getpwuid(uid)) == NULL) {
		sprintf(numstr, "%d", uid);
		return numstr;
	}
	else
		return pw_ptr->pw_name;
}

char* gid_to_name (gid_t gid) {
	struct group *getgrid(), *grp_ptr;
	static char numstr[10];
	if((grp_ptr = getgrgid(gid)) == NULL) {
		sprintf(numstr, "%d", gid);
		return numstr;
	}
	else
		return grp_ptr->gr_name;
}
