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

void printBin(unsigned char c);




int main() {

	//printf("Enter picture name or full path:\n");
	char picture[100] = "car.jpg";
	//scanf("%99s",picture);








	/* wait for rotation value in /dev/ttyUSB0 */

/*
	int file;
    struct termios options;
	char port[] = "/dev/ttyUSB0";

	file = open(port, O_RDONLY | O_NOCTTY | O_NDELAY);

	if(file == -1) perror("Unable to open the serial port\n"); 
    printf("Serial port opened successfully\n");


	tcgetattr(file, &options);          
    cfsetispeed(&options, B9600);                   
    cfsetospeed(&options, B9600); 
//          
    options.c_cflag |= (CLOCAL | CREAD);              
    options.c_cflag |= PARENB;                      //No parity                 
    options.c_cflag |= PARODD;                      
    options.c_cflag &= ~CSTOPB;                     
    options.c_cflag &= ~CSIZE;                      
    options.c_cflag |= CS8;                         //8 bits                   
    tcsetattr(file, TCSANOW, &options);          
    fcntl(file, F_SETFL, FNDELAY);          
//
	// Enable the receiver and set local mode...
	options.c_cflag |= (CLOCAL | CREAD);
	// Set 8-bit mode
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;	

	tcsetattr(file, TCSANOW, &options);       
    fcntl(file, F_SETFL, FNDELAY); 
*/

	int file;
    struct termios SerialPortSettings;
	char port[] = "/dev/ttyUSB0";

	file = open(port, O_RDONLY | O_NOCTTY);

	if(file == -1) perror("Unable to open the serial port\n"); 
    printf("Serial port opened successfully\n");

	tcgetattr(file, &SerialPortSettings);	/* Get the current attributes of the Serial port */
	cfsetispeed(&SerialPortSettings,B9600); /* Set Read  Speed as 9600  */
	cfsetospeed(&SerialPortSettings,B9600); /* Set Write Speed as 9600  */
	SerialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
	SerialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
	SerialPortSettings.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size             */
	SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */

	//SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
	//SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */ 

	//SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
	//SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode  */

	//SerialPortSettings.c_cc[VMIN] = 10; /* Read at least 10 characters */
	SerialPortSettings.c_cc[VTIME] = 0; /* Wait indefinetly   */

	if((tcsetattr(file,TCSANOW,&SerialPortSettings)) != 0) /* Set the attributes to the termios structure*/
		printf("\n  ERROR ! in Setting attributes");
	//fcntl(file, F_SETFL, FNDELAY);
	tcflush(file, TCIFLUSH);

    printf("Reading serial port ...\n\n");
	char data[10];
	int bytes = -1;
	bytes = read(file,&data,5);
    if (bytes < 0) {
        printf("reception error[%d]: %s\n",errno,strerror(errno));
        return -1;
    }
	printf("Read %d bytes\n",bytes);
    close(file);
    printf("Serial port closed\n");
	printf("Read: [%s]\n",data);
	int k = 0;
	while (k++ < bytes)
		printBin(data[k]);








	/* Convert picture */
	char path[] = "/usr/bin/convert";
	char* args_convert[20];
	int i = 0;
	args_convert[i++] = "convert";
	args_convert[i++] = picture;
	args_convert[i++] = "-rotate";
	args_convert[i++] = "270";
	args_convert[i++] = "-resize";
	args_convert[i++] = "84x48";
	args_convert[i++] = "-background";
	args_convert[i++] = "white";
	args_convert[i++] = "-gravity";
	args_convert[i++] = "center";
	args_convert[i++] = "-extent";
	args_convert[i++] = "84x48";
	args_convert[i++] = "-colorspace";
	args_convert[i++] = "gray";
	args_convert[i++] = "-monochrome";
	args_convert[i++] = "-depth";
	args_convert[i++] = "1";
	args_convert[i++] = "pictmp.bmp";
	int pid_convert = fork();
	if(!pid_convert) {
		printf("**Forked convert\n");
		execve("/usr/bin/convert",args_convert, NULL);
		printf("Error converting :[\n");
		return -1;
	}
	int status;
	do {
		waitpid(pid_convert, &status, 0);
	} while(!WIFEXITED(status));
	
	/* Format picture */
	char* args_format[20];
	args_format[0] = "format";
	args_format[1] = "pictmp.bmp";
	args_format[2] = NULL;
	int pid_format = fork();
	if(!pid_format) {
		printf("**Forked format\n");
		execve("/home/vlad_ko/Documents/Embedded/STM32F103/format",args_format, NULL);
		printf("Error formatting :[\n");
		return -1;
	}
	do {
		waitpid(pid_format, &status, 0);
	} while(!WIFEXITED(status));

	/* Send result to /dev/ttyUSB0 */
/*
	socat OPEN:/home/vlad_ko/Documents/Embedded/STM32F103/result /dev/ttyUSB0,b9600,raw,echo=0
*/
	int pid_socat = fork();
	char* args_socat[100];
	args_socat[0] = "socat";
	args_socat[1] = "OPEN:/home/vlad_ko/Documents/Embedded/STM32F103/result";
	args_socat[2] = "/dev/ttyUSB0,b9600,raw";
	args_socat[3] = NULL;
	if(!pid_socat) {
		printf("**Forked socat\n");
		execve("/usr/bin/socat",args_socat, NULL);
		printf("Error sending :[\n");
		return -1;
	}
	
	do {
		waitpid(pid_socat, &status, 0);
	} while(!WIFEXITED(status));

	return 0;
}



void printBin(unsigned char c) {
	int a = (int)c;
	int b = 2;
	int i = 0;
	unsigned char rests[10];

	while(a >= b) {
		rests[i] = (unsigned char)(a % b);
		a = a / b;
		i++;
	}
	rests[i] = (unsigned char)(a);

	for(int j = 7; j > i; j--) {
		printf("0");
	}
	for(int j = i; j >= 0; j--) {
		printf("%X",rests[j]);
	}
	printf(" ");
}
