#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

//터미널의 속성 정보 저장
//입력 및 로컬 플래그는 구조체로 정의됩니다.
struct flaginfo {
	int fl_value;
	char* fl_name;
};


void str_to_upper(char* str);
//Flip() 함수와 AllFlip() 함수는 입력 및 로컬 속성 플래그를 토글하고 설정
void Flip(struct termios* ttyp, struct flaginfo thebitnames[], char* argv);
void AllFlip(struct termios* ttyp, struct flaginfo thebitnames[]);
void showbaud(int thespeed);
void show_some_flags(struct termios* ttyp, int flag);
void show_flagset(int thevalue, struct flaginfo thebitnames[], int flag);

struct flaginfo input_flags[] = {
	IGNBRK, "ignbrk",
	BRKINT, "brkint",
	IGNPAR, "ignpar",
	PARMRK, "parmrk",
	INPCK, "inpck",
	ISTRIP, "istrip",
	INLCR, "inlcr",
	IGNCR, "igncr",
	ICRNL, "icrnl",
	IXON, "ixon",
	IXOFF, "ixoff",
	0, NULL };

struct flaginfo local_flags[] = {
	ISIG, "isig",
	ICANON, "icanon",
	ECHO, "echo",
	ECHOE, "echoe",
	ECHOK, "echok",
	0,NULL };

int main(int argc, char* argv[]) {
	struct termios ttyinfo;
	int flag = 0;
	//tcgetattr()= 현재 터미널의 속성 정보를 가져옴
	if (tcgetattr(0, &ttyinfo) == -1) {
		perror("cannot get params about stdin");
		exit(1);
	}
	//cfgetospeed() = 통신 속도 정보 출력
	showbaud(cfgetospeed(&ttyinfo));
	//typeinfo.c_cc 배열을 사용하여 백스페이스 및 삭제 등의 특수한 입력 문자에 대한 정보 출력
	printf("erase = ^%c;kill = ^%c\n", ttyinfo.c_cc[VERASE] - 1 + 'A', ttyinfo.c_cc[VKILL] - 1 + 'A');

	if (argc > 1) {
		if (tcgetattr(0, &ttyinfo) == -1)
			perror("tcgetattr");

		if (strcmp(argv[1], "-FI") == 0) {
			int i = 2;
			while (i < argc) {
				Flip(&ttyinfo, input_flags, argv[i]);
				i++;
			}
		}
		else if (strcmp(argv[1], "-FO") == 0) {
			if (flag == 1)	flag = 0;
			else if (flag == 0)	flag = 1;
		}

		else if (strcmp(argv[1], "-FL") == 0) {
			int j = 2;
			while (j < argc) {
				Flip(&ttyinfo, local_flags, argv[j]);
				j++;
			}
		}

		else if (strcmp(argv[1], "-A") == 0) {
			if (flag == 1)	flag = 0;
			else if (flag == 0)	flag = 1;
			AllFlip(&ttyinfo, input_flags);
			AllFlip(&ttyinfo, local_flags);
		}

		if (tcsetattr(0, TCSANOW, &ttyinfo) == -1)
			perror("tcsetattr");

	}
	show_some_flags(&ttyinfo, flag);
}

void Flip(struct termios* ttyp, struct flaginfo thebitnames[], char* argv) {

	 int thevalue = ttyp->c_lflag;
	for (int i = 0; thebitnames[i].fl_value; i++) {
		if (strcmp(argv, thebitnames[i].fl_name) == 0) {
			if (thevalue & thebitnames[i].fl_value) {
				 ttyp->c_lflag &= ~thebitnames[i].fl_value; 
			}
			else {
				 ttyp->c_lflag |= thebitnames[i].fl_value;
			}
		}
	}

}

void AllFlip(struct termios* ttyp, struct flaginfo thebitnames[]) {

	int thevalue = ttyp->c_lflag;

	for (int i = 0; thebitnames[i].fl_value; i++) {
		if (thevalue & thebitnames[i].fl_value) {
			ttyp->c_lflag |= ECHO;
		}
		else {
			ttyp->c_lflag &= ~ECHO;
		}
	}
}


void showbaud(int thespeed) {
	printf("the baud rate is");
	switch (thespeed) {
	case B300:  printf("300\n"); break;
	case B600:  printf("600\n"); break;
	case B1200: printf("1200\n"); break;
	case B1800: printf("1800\n"); break;
	case B2400: printf("2400\n"); break;
	case B4800: printf("4800\n"); break;
	case B9600: printf("9600\n"); break;
	default:    printf("Fast\n"); break;
	}
}



void show_some_flags(struct termios* ttyp,int flag) {
	show_flagset(ttyp->c_lflag, input_flags, flag);
	show_flagset(ttyp->c_lflag, local_flags, flag);
}

void show_flagset(int thevalue, struct flaginfo thebitnames[], int flag) {
	int i;
	char dest[10];
	for (i = 0; thebitnames[i].fl_value; i++) {
		if (thevalue & thebitnames[i].fl_value) {
		}
		else {
			printf("-");
		}
		if (flag == 1) {
			strcpy(dest, thebitnames[i].fl_name);
			str_to_upper(dest);
			printf("%s ", dest);
		}
		else {
			printf("%s ", thebitnames[i].fl_name);
		}
	}
	printf("\n");
}

void str_to_upper(char* str) {
	while (*str) {
		*str = toupper(*str);
		str++;
	}
}