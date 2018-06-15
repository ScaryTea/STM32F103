#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

/*
convert car.jpg -rotate 90 -resize 84x48 -background white -gravity center -extent 84x48 -colorspace gray -monochrome -depth 1 carsize.bmp
*/

void kill_old();
char* wait_angle();
void serial_config();
void convert_pic(char* picture, char* angle);
void format_pic();
void send_pic();
void send_pic2();

int main() {
	//kill_old();
	printf("Enter picture name or full path:\n");
	char picture[100];
	scanf("%99s",picture);

	char* angle = "0";

	int pid = fork();

	if(pid) { // write child's pid to file
		FILE* f = fopen("tmp/pic_prev_pid","w");
		fwrite(&pid, sizeof(int), 1, f);
		fclose(f);
	}

	if(!pid) {
		while(1) {
			convert_pic(picture, angle);
			format_pic();
			send_pic2();
			angle = wait_angle();
	//		printf("Received angle: [%s]\n",angle);
		}
	}

	return 0;
}

void kill_old() {
	printf("YYYYYYYYYYYYY");
	FILE* f = fopen("/tmp/pic_prev_pid","r");
	if(f < 0) {
		printf("Unable to open tmp file\n");
	}
	int prev_pid = -1;
	if(fread(&prev_pid, sizeof(int), 1, f))
		printf("prev_pid: %d, kill result: %d\n",prev_pid,kill(prev_pid, SIGKILL));
	fclose(f);
}

char* wait_angle() {
	int file;

	file = open("/dev/ttyUSB0", O_RDONLY | O_NOCTTY );
	if(file == -1) { 
		perror("Unable to open the serial port\n");
	}
//	printf("Serial port opened successfully\n");

	struct termios options;
	tcgetattr(file, &options);          
	cfsetispeed(&options, B9600);                   
	cfsetospeed(&options, B9600);                   
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag |= PARENB;	//No parity                 
	//options.c_cflag |= PARODD;                      
	options.c_cflag &= ~CSTOPB;                     
	options.c_cflag &= ~CSIZE;                      
	options.c_cflag |= CS8;		//8 bits  
	cfmakeraw(&options);
	tcsetattr(file, TCSANOW, &options);


	int bytes = -1;
	char data[3];
//	printf("Reading serial port ...\n\n");
	bytes = read(file, data, 2);
	if (bytes < 0)
		printf("Read error[%d]: %s\n",errno,strerror(errno));
	char* angle;
//	printf("DATA = [%s]\n",data);
	switch (data[0]) {
		case '0': angle = "0"; break;
		case '1': angle = "90"; break;
		case '2': angle = "180"; break;
		case '3': angle = "270"; break;
		default: angle =  "270"; break;
	}
	close(file);
	return angle;
}

void convert_pic(char* picture, char* angle) {
	char path[] = "/usr/bin/convert";
	char* args_convert[20] = {"convert",picture,"-rotate",angle,"-resize","84x48",
	"-background", "white", "-gravity", "center", "-extent", "84x48", "-colorspace",
	"gray", "-monochrome", "-depth", "1", "pictmp.bmp"};
	int pid_convert = fork();
	if(!pid_convert) {
//		printf("Started converting...\n");
		execve("/usr/bin/convert",args_convert, NULL);
//		printf("Error converting :[\n");
		return;
	}
	int status;
	do {
		waitpid(pid_convert, &status, 0);
	} while(!WIFEXITED(status));
//	printf("Converting finished!\n");
}

void format_pic() {
	char* args_format[20] = {"format","pictmp.bmp",NULL};
	int pid_format = fork();
	if(!pid_format) {
//		printf("Started formatting...\n");
		execve("/home/vlad_ko/Documents/Embedded/STM32F103/format",args_format, NULL);
//		printf("Error formatting :[\n");
		return;
	}
	int status;
	do {
		waitpid(pid_format, &status, 0);
	} while(!WIFEXITED(status));
//	printf("Formatting finished!\n");
}

void send_pic() {
/* socat OPEN:/home/vlad_ko/Documents/Embedded/STM32F103/result /dev/ttyUSB0,b9600,raw,echo=0 */
	int pid_socat = fork();
	char* args_socat[100] = {"socat",
	"OPEN:/home/vlad_ko/Documents/Embedded/STM32F103/result", "/dev/ttyUSB0,b9600,raw",NULL};
	if(!pid_socat) {
//		printf("Started sending...\n");
		execve("/usr/bin/socat",args_socat, NULL);
//		printf("Error sending :[\n");
		return;
	}
	int status;
	do {
		waitpid(pid_socat, &status, 0);
	} while(!WIFEXITED(status));
//	printf("Sent successfully!\n");
}


void send_pic2() {
	int file;

	file = open("/dev/ttyUSB0", O_WRONLY | O_NOCTTY );
	if(file == -1) { 
		perror("Unable to open the serial port\n");
	}

	struct termios options;
	tcgetattr(file, &options);          
	cfsetispeed(&options, B9600);                   
	cfsetospeed(&options, B9600);                   
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag |= PARENB;	//No parity                     
	options.c_cflag &= ~CSTOPB;                     
	options.c_cflag &= ~CSIZE;                      
	options.c_cflag |= CS8;		//8 bits  
	cfmakeraw(&options);
	tcsetattr(file, TCSANOW, &options);


	FILE* f = fopen("/home/vlad_ko/Documents/Embedded/STM32F103/result","r");
	char data[505];
	//printf("[%u] bytes read\n\n",fread(data, sizeof(char), 505, f));
	//printf("data read from result: [[%s]]\n",data);
	int bytes = write(file, data, 504);
	//printf("bytes written: [%d]\n",bytes);
	//printf("error(%d): %s\n",errno, strerror(errno));
	fclose(f);
	close(file);
	printf("Sent successfully!\n");
}

