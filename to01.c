#include <stdio.h>
#include <stdlib.h>

void printBin(unsigned char c);

unsigned long counter = 0;

int main(int argc, char *argv[]) {
	
	printf("argv[0] = %s, argv[1] = %s\n", argv[0], argv[1]);
	FILE* f = fopen(argv[1], "r");
	if(f == NULL) 
	{
		printf("Error openning the file\n");
		return 1;
	}
	fseek(f, 0, SEEK_END);
	int fsize = ftell(f);
	rewind(f);
	unsigned char* input = (unsigned char*) malloc(sizeof(unsigned char)*fsize);
	fread(input, sizeof(unsigned char), fsize, f);
	printf("Read file: [%s]\n",input);
	for(int i = 0; i < fsize; i++)
		printBin(input[i]);
	printf("\n");
	fclose(f);
	printf("Totally %lu bits\n",counter);
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
		counter++;
	}
	for(int j = i; j >= 0; j--) {
		printf("%X",rests[j]);
		counter++;
	}
	printf(" ");
}
