#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

uint8_t shifts[] = { 1<<7, 1<<6, 1<<5, 1<<4, 1<<3, 1<<2, 1<<1, 1 };

uint32_t fsize = 0;
unsigned char* readFile(char* path);
void printBin(unsigned char c);

int main(int argc, char *argv[]) {

	unsigned char* input = readFile(argv[1]);
	printf("file = %s\n",input);
	uint32_t offset = input[10];
	uint32_t width = input[18];
	uint32_t height = input[22];
	unsigned char arr[6][84];
	for(int i = 0; i < 6; i++)
		for(int j = 0; j < 84; j++)
			arr[i][j] = 0;
	printf("res: (%u)\n fsize = %d\n",offset,fsize);
//	int k = fsize - 1;

	unsigned char a;	
	int i=0, j=83;
	for(int v = 0; v < 48; v++, j=83, i=(int)(v/8)) {
		//printf("v=%d,j=%d,i=%d\n",v,j,i);
		for(int k = 1; k < 12; k++) {
			//printf("k=%d,j=%d\n",k,j);
			a = input[fsize-1 - v*12 - k];
			//printBin(a);
			for(int tmp = 0; tmp < 8; tmp++, j--) {
				//printf("lowloop: j=%d, tmp=%d\n",j,tmp);
				if(j < 0) break;
				//printf("\n[[arr[%d][%d] before: ",i,j);
				//printBin(arr[i][j]);
				arr[i][j] |= (!!(a & (1 << tmp))) << v%8;
				/*printf("\narr[%d][%d] after: ",i,j);
				printBin(arr[i][j]);
				printf("\na(): ");
				printBin(a);
				printf("\nv=%d; tmp=%d; k=%d",v,tmp,k);
				printf("]]\n");*/
			}
		}
	}
	FILE* f = fopen("result", "w+");
	for(i = 0; i < 6; i++)
		for(j = 0; j < 84; j++) {
			fwrite(&arr[i][j], sizeof(unsigned char), 1, f);
		}
	fclose(f);

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
