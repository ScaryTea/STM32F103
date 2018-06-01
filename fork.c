#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#define args_len 200

void flush_input() {
    int c;
    while((c = getchar()) != '\n' );
}

int main() {
	printf("Enter path to process to launch:\n");
	char path[args_len];
	scanf("%199s",path);

	flush_input();

	char* args[20];
	int i = 0;
	args[i] = "child_process";
	printf("Enter args:\n");
	do {
		args[++i] = malloc(sizeof(char[20]));
		fgets(args[i],19,stdin);
		if(args[i][0] == '\n') 
			break;
		int k = 0;
		while(args[i][k++] != '\n');
		args[i][k-1] = '\0';
	} while(1); 
	args[i-1] = NULL;

	// use fork to create new process
	int pid = fork();
	if(pid == -1) {
		printf("Error creating new process :[\n");
		return -1;
	}
	
	// use execve to replace context
	if(!pid) {
		execve(path, args, NULL);
		printf("No such file :[\n");
		return -1;
	}

	// get return code of the child process and print everythn
	int status;
	do {
		waitpid(pid, &status, 0);
	} while(!WIFEXITED(status));
	printf("parent pid: %d\n",getpid());
	printf("child pid: %d\n",pid);
	printf("status returned: %d\n",WEXITSTATUS(status));
	return 0;
}
