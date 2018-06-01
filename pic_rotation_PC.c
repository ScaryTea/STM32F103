#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <string.h>

/*
convert car.jpg -rotate 90 -resize 84x48 -background white -gravity center -extent 84x48 -colorspace gray -monochrome -depth 1 carsize.bmp
*/

char* wait_angle();
void serial_config();
void convert_pic(char* picture, char* angle);
void format_pic();
void send_pic();

int main() {

	printf("Enter picture name or full path:\n");
	char picture[100];
	scanf("%99s",picture);
	
	char* angle = "0";

	while(1) {
		convert_pic(picture, angle);
		format_pic();
		send_pic();
		angle = wait_angle();
		printf("Received angle: [%s]\n",angle); /* */
	}
	return 0;
}

char* wait_angle() {
	int file;

	file = open("/dev/ttyUSB0", O_RDONLY | O_NOCTTY );
	if(file == -1) { 
		perror("Unable to open the serial port\n");
	}
	printf("Serial port opened successfully\n");

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
	printf("Reading serial port ...\n\n");
	bytes = read(file, data, 2);
	if (bytes < 0)
		printf("Read error[%d]: %s\n",errno,strerror(errno));
	char* angle;
	printf("DATA = [%s]\n",data);
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
		printf("Started converting...\n");
		execve("/usr/bin/convert",args_convert, NULL);
		printf("Error converting :[\n");
		return;
	}
	int status;
	do {
		waitpid(pid_convert, &status, 0);
	} while(!WIFEXITED(status));
	printf("Converting finished!\n");
}

void format_pic() {
	char* args_format[20] = {"format","pictmp.bmp",NULL};
	int pid_format = fork();
	if(!pid_format) {
		printf("Started formatting...\n");
		execve("/home/vlad_ko/Documents/Embedded/STM32F103/format",args_format, NULL);
		printf("Error formatting :[\n");
		return;
	}
	int status;
	do {
		waitpid(pid_format, &status, 0);
	} while(!WIFEXITED(status));
	printf("Formatting finished!\n");
}

void send_pic() {
/* socat OPEN:/home/vlad_ko/Documents/Embedded/STM32F103/result /dev/ttyUSB0,b9600,raw,echo=0 */
	int pid_socat = fork();
	char* args_socat[100] = {"socat",
	"OPEN:/home/vlad_ko/Documents/Embedded/STM32F103/result", "/dev/ttyUSB0,b9600,raw",NULL};
	if(!pid_socat) {
		printf("Started sending...\n");
		execve("/usr/bin/socat",args_socat, NULL);
		printf("Error sending :[\n");
		return;
	}
	int status;
	do {
		waitpid(pid_socat, &status, 0);
	} while(!WIFEXITED(status));
	printf("Sent successfully!\n");
}

