#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "loader.h"
#include "auxiliary.h"

PRCX8 bootstrap(const char * name){
	FILE * pIn = fopen(name, "rb");
	if(pIn != NULL){
		printf("senf\n");
		WORD buf;
		PRCX8 retVal = (PRCX8) malloc(sizeof(RCX8));
		setup(retVal);
		int mode = 0;
		while(!feof(pIn) && mode==0){
			if(fread(&buf,sizeof(buf),1,pIn) < 1)
			  return NULL;
			//printf("0x%x\n",buf);
			switch(buf){
				case 0x0000:
					printf("EOF\n");
					mode=1;
					break;
				case 0x1000:
					printf("found comment block\n");
					char c;
					fread(&buf,sizeof(buf),1,pIn);
					for(int i = 0; i < buf; i++){
						fread(&c, sizeof(c),1,pIn);
						printf("%c", c);
					}
					printf("\n");
					//fseek(pIn, (long int) buf, SEEK_CUR);
					break;
				case 0x2000:
					fread(&buf,sizeof(buf),1,pIn); //Loading offset
					ADDR offset = buf;
					fread(&buf,sizeof(buf),1,pIn); //Length of data
					WORD length = buf;
					printf("found data block %x len: %d\n",offset,length);
					fread(retVal->memory + offset, sizeof(BYTE),length, pIn);
					break;
				case 0x7000:
					fread(&buf,sizeof(buf),1,pIn);
					printf("PC Address: %x\n", buf);
					retVal->pc = buf;
					break;
				}
		}
		fclose(pIn);
		return retVal;
	}
	return NULL;
}


void setup(PRCX8 state){
	if(state != NULL){
	memset(state->reg,0,_REGISTERCOUNT_*sizeof(UBYTE));
    memset(state->memory,0,_MEMSIZE_ * sizeof(UBYTE));
	memset(state->stack,0,_CALLSTACKSIZE_*sizeof(ADDR));
	state->spointer = 0;
	state->acc = 0;
	state->pc = 0;
	state->carry = 0;
	state->run = 1;
	}
}

void dumpstate(PRCX8 state){
	FILE * fout;
	fout = fopen("dump.out","wb");
	if(fout != NULL){
		fprintf(fout,"PC:%p\nACC:%p\n\nMEMORY:\n",&state->pc,&state->acc);
		fwrite(state->memory,sizeof(UBYTE),_MEMSIZE_, fout);
		fclose(fout);
	}
	
}
			
