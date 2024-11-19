#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

void do_ls(char[]);
void dostat(char*);
void show_time_info(char*, struct stat*);
void mode_to_letters(int, char[]);
char* uid_to_name(uid_t);
char* gid_to_name(gid_t);

int main(int argc, char* argv[]) {

    char dst_dir[1024];
    char src_file[1024];
    char dst_file[1024];
    int directoryflag = 0;
    int fileflag = 0;

    if(argc >= 1 && argc < 5){
	    perror("Destination directory does not exist\n");
	    exit(EXIT_FAILURE);
    }
     else if (argc != 5){
        if (strcmp(argv[1], "-d") != 0 || strcmp(argv[3], "-e") != 0) {
            perror("Destination directory does not exist\n");
            exit(EXIT_FAILURE);
        }
        if (argc > 5) {
            fprintf(stderr, "%s: invalid option -- '%s'\n",argv[0], argv[6]);
	    fprintf(stderr,"Usage: %s -d destination_directory -e extension\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    //과제에서 주어진 my_mv 실행
    else if (strcmp(argv[1], "-d") == 0 && strcmp(argv[3], "-e") == 0)
    {
        //특정 디렉토리 명을 찾기 위해 현재 디렉토리 열기
        DIR* dir = opendir(".");
        if (dir == NULL) {
            perror("Destination directory does not exist\n");
            exit(EXIT_FAILURE);
        }

        //입력한 이름으로 디렉토리 찾기
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, argv[2]) == 0) {
                snprintf(dst_dir, sizeof(dst_dir), "./%s", argv[2]);
		directoryflag++;
                if (realpath(dst_dir, dst_dir) == NULL) {
                    //에러 메시지 출력
                    perror("Destination directory does not exist\n");
                    exit(EXIT_FAILURE);
                }
                break;
            }
        }
	if(directoryflag == 0){
		perror("Destination directory does not exist\n");
		exit(EXIT_FAILURE);
	}
        closedir(dir);

        //확장자로 파일을 찾기 위하여 현재 디렉토리 열기
        dir = opendir(".");
        if (dir == NULL) {
            perror("No files with extension png found\n");
            exit(EXIT_FAILURE);
        }
      
	if ((entry = readdir(dir)) == NULL){
		fprintf(stderr,"No files with extension %s found\n", argv[4]);
	        exit(EXIT_FAILURE);
	}

        //특정 확장자 명을 가진 파일 이동시키기
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_REG && strstr(entry->d_name, argv[4]) != NULL) {
                snprintf(src_file, sizeof(src_file), "./%s", entry->d_name);
                snprintf(dst_file, sizeof(dst_file), "%s/%s", dst_dir, entry->d_name);
                if (rename(src_file, dst_file) != 0) {
                    fprintf(stderr,"No files with extension %s found\n", argv[4]);
                    exit(EXIT_FAILURE);
                }
		fileflag++;
            }
            
        }
	if(fileflag == 0) 
	{
		fprintf(stderr, "No files with extension %s found\n", argv[4]);
		exit(EXIT_FAILURE);
	}
        closedir(dir);

    }
}

// 지정된 디렉토리 내의 파일 목록 출력
void do_ls(char dirname[]) {
    DIR* dir_ptr;
    struct dirent* direntp;
    char path[256];

    if (chdir(dirname) != 0) {
        perror(dirname);
        exit(1);
    }

    getcwd(path, sizeof(path));
    printf("path : %s\n", path);
    printf("dirname : %s\n", dirname);

    if ((dir_ptr = opendir(path)) == NULL)
        fprintf(stderr, "ls: cannot open%s\n", path);
    else {
        while ((direntp = readdir(dir_ptr)) != NULL) {
            if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
                continue;
            dostat(direntp->d_name);
        }closedir(dir_ptr);
    }
}

// 파일 정보를 출력
void dostat(char* filename) {
    struct stat info;
    if (stat(filename, &info) == -1)
        perror(filename);
    else
        show_file_info(filename, &info);
}

void show_file_info(char* filename, struct stat* info_p) {
    char* uid_to_name(), * ctime(), * gid_to_name(), * filemode();
    void mode_to_letters();
    char modestr[] = "-------------";

    mode_to_letters(info_p->st_mode, modestr);
    printf("%s", modestr);
    printf("%4d ", (int)info_p->st_nlink);
    printf("%-8s ", uid_to_name(info_p->st_nlink));
    printf("%-8s ", gid_to_name(info_p->st_gid));
    printf("%-8ld ", (long)info_p->st_size);
    printf("%.12s ", 4 + ctime(&info_p->st_mtime));
    printf("%s \n", filename);
}

void mode_to_letters(int mode, char str[]) {
    if (S_ISDIR(mode)) str[0] = 'd';
    if (S_ISCHR(mode)) str[0] = 'c';
    if (S_ISBLK(mode)) str[0] = 'd';

    if (mode & S_IRUSR) str[1] = 'r';
    if (mode & S_IWUSR) str[2] = 'w';
    if (mode & S_IXUSR) str[3] = 'x';

    if (mode & S_IRGRP) str[4] = 'r';
    if (mode & S_IWGRP) str[5] = 'w';
    if (mode & S_IXGRP) str[6] = 'x';

    if (mode & S_IROTH) str[7] = 'r';
    if (mode & S_IWOTH) str[8] = 'w';
    if (mode & S_IXOTH) str[9] = 'x';

}

char* uid_to_name(uid_t uid) {
    struct passwd* getpwuid(), * pw_ptr;
    static char numstr[10];

    if ((pw_ptr = getpwuid(uid)) == NULL) {
        sprintf(numstr, "%d", uid);
        return numstr;
    }
    else return pw_ptr->pw_name;
}

char* gid_to_name(gid_t gid) {
    struct group* getgrid(), * grp_ptr;
    static char numstr[10];
    if ((grp_ptr = getgrgid(gid)) == NULL) {
        sprintf(numstr, "%d", gid);
        return numstr;
    }
    else
        return grp_ptr->gr_name;        
}
​