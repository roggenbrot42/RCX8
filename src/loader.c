#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "loader.h"
#include "auxiliary.h"

PRCX8 bootstrap(const char * name){
	FILE * pIn = fopen(name, "rb");
	if(pIn != NULL){
		printf("senf");
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
					mode=1;
					break;
				case 0x1000:
					fread(&buf,sizeof(buf),1,pIn);
					fseek(pIn, (long int) buf, SEEK_CUR);
					break;
				case 0x2000:
					fread(&buf,sizeof(buf),1,pIn);
					ADDR offset = buf;
					fread(&buf,sizeof(buf),1,pIn);
					WORD length = buf;
					BYTE bytebuf;
					int i=0;
					for(i;i<length;i++){ //TODO replace by memcpy!
						fread(&bytebuf,sizeof(bytebuf),1,pIn);
						retVal->memory[i+offset] = bytebuf;
					}
					break;
				case 0x7000:
					fread(&buf,sizeof(buf),1,pIn);
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
        memset(state->memory,1,_MEMSIZE_ * sizeof(UBYTE));
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
			
