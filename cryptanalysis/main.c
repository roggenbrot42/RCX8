#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define _BUFSIZE_ 255
#define _OFFSETUP_ 65
#define _OFFSETLO_ 97

void shiftString(char ** in, char ** out, long lSize, unsigned int shift){
	int offset = 0;
	int i = 0;
	char * c, *o;
	for(i = 0; i < lSize; i++){
		c = *in+i;
		o = *out+i;
		if(isalpha(*c)){
			offset = (islower(*c))?_OFFSETLO_:_OFFSETUP_;
			*o =  ((*c - offset)%26 + shift)%26 + offset;
		}
	}
}

void createASCIIHistogram(char * in, long lSize, int histogram[255]){	
	for(;lSize>0;--lSize){
		char c = in[lSize];
		histogram[c]++;
	}
}

int main(int argc, char * argv[]){

	FILE * fin = 0;
	char inFileName[_BUFSIZE_];
	unsigned int shift = 0;
	
	if(argc < 2){
		printf("Usage: crypta filename\n");
		return 0;
	}
	else if(argc == 3){
		fin = fopen(argv[1],"r");
		if(fin == NULL){
			printf("Invalid filename!\n");
			exit(2);
		}
		shift = strtoul(argv[2],NULL,0);
		if(shift == 0){
			fputs("Invalid argument, must be positive integer\n", stderr);
			exit(2);
		}
	}
	else{
		printf("Invalid number of parameters given. Stick to the rules\n");
		exit(1);
	}
	
	char * buffer;
	long lSize;
	
	fseek(fin, 0 , SEEK_END);
	lSize = ftell(fin);
	rewind(fin);
	
	buffer = (char*) malloc (sizeof(char)*lSize);
	if(buffer == NULL) { fputs ("Memory error\n", stderr); exit(2); }
	
	fread(buffer, 1, lSize, fin);

	unsigned int histogram[255];
	memset(histogram,0,255*sizeof(int));
	createASCIIHistogram(buffer, lSize, histogram);
	shiftString(&buffer,&buffer,lSize,shift);
	

	
	int i=0;
	for(; i < 255; i++){
		if(isgraph((char)i))
			printf("%c: %d\n", (char) i, histogram[i]);
	}
	
	fclose(fin);
	
	FILE * fout = 0;
	fout = fopen("out.txt", "w+");
	if(fout == 0){
		fputs("Error writing to file\n", stderr);
		exit(2);
	}
	else{
		fwrite(buffer, 1, lSize, fout);
	}
	fclose(fout);
	
	return 0;
}

