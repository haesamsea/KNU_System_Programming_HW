//헤더
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>


void do_ls(char[]);
void dostat(char*);
void show_time_info(char*, struct stat*);
void mode_to_letters(int, char[]);
char* uid_to_name(uid_t);
char* gid_to_name(gid_t);
void listDir(int);

// 명령행 인수로 받은 값에 따라 do_ls 함수 실행
// sizeOrder 변수는 파일 크기를 기준으로 정렬할 때 
// 오름차순 또는 내림차순으로 정렬하기 위한 변수
int main(int argc, char*argv[]){

	int sizeOrder = 1;
	if (argc == 1)
		do_ls(".");
	else{
		if(strcmp(argv[1], "-S") == 0) {
			sizeOrder = 1;	
			listDir(sizeOrder);
			}
		else if (argc>1 && strcmp(argv[1], "-s") == 0){
			sizeOrder = -1;
			listDir(sizeOrder);
		}
		else{
			while(--argc){
				printf("%s:\n", *++argv);
				do_ls(*argv);
			}
		}
	}
	return 0;
}

//파일 크기를 기준으로 출력
void listDir(int sizeOrder) {
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;

    dir = opendir(".");
    if (!dir) {
        printf("Failed to open directory.");
        return;
    }

    int n = 0;
    char **filenames = NULL;
// readdir = 디렉토리 내 파일 읽어들이기
    while ((entry = readdir(dir))) {
        if (entry->d_name[0] != '.') {
            char *filename = malloc(strlen(entry->d_name) + 1);
            strcpy(filename, entry->d_name);
            filenames = realloc(filenames, (n + 1) * sizeof(char *));
            filenames[n++] = filename;
        }
    }
    closedir(dir);

    int i, j;
    char *temp;
    for (i = 0; i < n - 1; i++) {
        for (j = i + 1; j < n; j++) {
            stat(filenames[i], &fileStat);
            long size1 = fileStat.st_size;
            stat(filenames[j], &fileStat);
            long size2 = fileStat.st_size;
            if (sizeOrder * size1 < sizeOrder * size2) {
                temp = filenames[i];
                filenames[i] = filenames[j];
                filenames[j] = temp;
            }
        }
    }

    for (i = 0; i < n; i++) {
// dostat() -> 파일 정보 출력
        dostat(filenames[i]);
        free(filenames[i]);
    }
    free(filenames);
}

void do_ls(char dirname[]){
	DIR *dir_ptr;
	struct dirent *direntp;
	char path[256];

	if (chdir(dirname)!= 0){
		perror(dirname);
		exit(1);
	}

	getcwd(path, sizeof(path));
	printf("path : %s\n",path);
	printf("dirname : %s\n", dirname);

	if ((dir_ptr = opendir(path)) == NULL)
		fprintf(stderr, "ls: cannot open%s\n", path);
	else{
		while((direntp=readdir(dir_ptr))!=NULL){
			if (strcmp(direntp->d_name, ".")==0 ||strcmp(direntp->d_name, "..")==0)
				continue;
			dostat(direntp -> d_name);
		}closedir(dir_ptr);
	}
}

void dostat (char*filename){
	struct stat info;
// stat() 파일의 메타데이터 정보를 구함
	if(stat(filename, &info) == -1)
		perror(filename);
	else 
// 출력
		show_file_info(filename, &info);
}

// 주어진 파일에 대한 정보를 출력한다
void show_file_info(char*filename, struct stat* info_p){
//uid_to_name()(UID) gid_to_name()(GID) - 사용자 이름과 그룹이름 출력
// mode_to_letters() 함수를 사용하여 파일 권한 정보를 문자열로 변환
	char *uid_to_name(), *ctime(), *gid_to_name(), *filemode();
	void mode_to_letters();
	char modestr[] = "-------------";

	mode_to_letters(info_p->st_mode, modestr);
	printf("%s", modestr);
	printf("%4d ", (int) info_p->st_nlink);
	printf("%-8s ", uid_to_name(info_p->st_nlink));
	printf("%-8s ",gid_to_name(info_p->st_gid));
	printf("%-8ld ", (long)info_p->st_size);
	printf("%.12s ", 4+ctime(&info_p->st_mtime));
	printf("%s \n", filename);
}


void mode_to_letters(int mode, char str[]){
	if(S_ISDIR(mode)) str[0] = 'd';
	if(S_ISCHR(mode)) str[0] = 'c';
	if(S_ISBLK(mode)) str[0] = 'd';

	if (mode&S_IRUSR) str[1] = 'r';
	if (mode&S_IWUSR) str[2] = 'w';
	if (mode&S_IXUSR) str[3] = 'x';

	if (mode&S_IRGRP) str[4] = 'r';
	if (mode&S_IWGRP) str[5] = 'w';
	if (mode&S_IXGRP) str[6] = 'x';

	if (mode&S_IROTH) str[7] = 'r';
	if (mode&S_IWOTH) str[8] = 'w';
	if (mode&S_IXOTH) str[9] = 'x';

}

// 주어진 UID 값에 해당하는 사용자 이름을 찾아 출력
char* uid_to_name(uid_t uid){
	struct passwd *getpwuid(), *pw_ptr;
	static char numstr[10];

	if((pw_ptr = getpwuid(uid)) == NULL){
		sprintf(numstr, "%d", uid);
		return numstr;
	}
	else return pw_ptr->pw_name;
}

// 주어진 GID 값에 대해 해당하는 그룹 이름을 찾아 출력
char* gid_to_name (gid_t gid){
	struct group *getgrid(), *grp_ptr;
	static char numstr[10];
	if ((grp_ptr = getgrgid(gid)) == NULL){
		sprintf(numstr, "%d", gid);
		return numstr;
	}
	else 
		return grp_ptr->gr_name;
}
​