#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

uint8_t shifts[] = { 1, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7 };

uint32_t fsize = 0;
unsigned char* readFile(char* path);
void printBin(unsigned char c);

int main(int argc, char *argv[]) {
	/* unused info, probably will need this in future
	uint32_t width = input[18];
	uint32_t height = input[22];
	*/
	unsigned char* input = readFile(argv[1]);
	uint32_t offset = input[10];
	printf("offset: (%u)\n fsize = %d\n",offset,fsize);

	unsigned char arr[6][84];
	for(int i = 0; i < 6; i++)
		for(int j = 0; j < 84; j++)
			arr[i][j] = 0;

	unsigned char a;	
	int i=0, j=83;
	// v - vertical iterating
	// k - iterating over bytes in 'strings' of 12 bytes
	// bitpos - iterating over bits in byte
	for(int v = 0; v < 48; v++, j=83, i=(int)(v/8)) {
		for(int k = 1; k < 12; k++) {
			a = input[fsize-1 - v*12 - k];
			for(int bitpos = (k==1)?4:0; bitpos < 8; bitpos++, j--) {
				if(j < 0) { printf("Smth broken\n"); break; }
				arr[i][j] |= (!!(a & shifts[bitpos])) << v%8;
			}
		}
	}
	FILE* f = fopen("result", "w+");
	for(i = 0; i < 6; i++)
		for(j = 0; j < 84; j++) {
			fwrite(&arr[i][j], sizeof(unsigned char), 1, f);
		}
}

unsigned char* readFile(char* path) {
	FILE* f = fopen(path, "r");
	if(f == NULL) 
	{
		printf("Error openning the file\n");
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	rewind(f);
	unsigned char* res = (unsigned char*) malloc(sizeof(unsigned char)*fsize);
	fread(res, sizeof(unsigned char), fsize, f);
	fclose(f);
	return res;
}

void printBin(unsigned char c) {
	int a = (int)c;
	int b = 2; // base
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
}
