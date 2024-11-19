#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int i = 0;
int input = 0;
char number = 0;

void f(int signum){
	printf("\n============================\n");
	printf("Total number of printed HelloWorld!: %d\n", i);
	printf("============================\n");
	if(i%2!=0) exit(0);
}

void ff(int signum){
	printf("\n===========================\n");
	printf("Total numper of printed HelloWorld!: %d\n", i);
	printf("Number of HelloWorld! prints remaining: %d\n", input - i);
	printf("==============================\n");
	if(i%2!=0) exit(0);
	if(input-i == 0) exit(0);
}

int get_response(char *q)
{
	printf("Please specify the limit of HelloWorld! to print:");
	number = getchar();
	input = atoi(number);
	return number;
}

int main(int argc, char* argv[]){
	void f(int);
	void ff(int);
	
	if (argc == 3 && input <= 0){
	       	printf("Inavalid input. Please enter a positive integer.\n");
		exit(0);
	}

	if(argc == 1){
	       	signal(SIGINT, f);
	}

	else{
	       	if(argc != 3){
		printf("Please specify the limit of HelloWorld! to print:");
		scanf("%d", &input);
		}
		else if(argc ==3){
			input = atoi(argv[2]);
		}
		signal(SIGINT, ff);
	}	

	while(1){
		sleep(1);
		printf("Hello World\n");
		i++;
		if(argc > 1 && (input - i) == 0){
			printf("\n===============================\n");
			printf("Total number of printed HelloWorld: %d\n", i);
			printf("Number of HelloWorld! prints remaining: %d\n", input - i);
			printf("================================\n");
			exit(0);
		}
	}
}
​