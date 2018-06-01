#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define args_str_length 200

int main() {
	printf("Enter path to process to launch:\n");
	char path[100];
	scanf("%99s",path);

	// use fork to create new process
	int pid = fork();
	if(pid == -1) {
		printf("Error creating new process :[\n");
		return -1;
	}
	
	// use execve to replace context
	char* args[] = {"child_process", NULL };
	if(!pid) {
		execve(path, args, NULL);
		printf("No such file :[\n");
		return -1;
	}

	// get return code of the child process and print everythn
	int status;
	waitpid(pid, &status, 0);
	printf("parent pid: %d\n",getpid());
	printf("child pid: %d\n",pid);
	printf("status returned: %d\n",WEXITSTATUS(status));
	return 0;
}
