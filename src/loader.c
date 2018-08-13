#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "loader.h"
#include "program.h"
#include "auxiliary.h"

struct RCX8* bootstrap(const char * name){
	FILE * pIn = fopen(name, "rb");
	if(pIn != NULL){
		printf("Loader started\n");
		int16_t buf;
		struct RCX8* retVal = (struct RCX8*) malloc(sizeof(struct RCX8));
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
                    fread(&buf,sizeof(buf),1,pIn);
                    char c;
                    printf("Found comment block at %x of length: %d\n", ftell(pIn),buf);
					for(int i = 0; i < buf; i++){
						fread(&c, sizeof(c),1,pIn);
						printf("%c", c);
					}
					printf("\n");
					//fseek(pIn, (long int) buf, SEEK_CUR);
					break;
				case 0x2000:
					fread(&buf,sizeof(buf),1,pIn); //Loading offset
					uint16_t offset = buf;
					fread(&buf,sizeof(buf),1,pIn); //Length of data
					int16_t length = buf;
					printf("Found data block %x at %x len: %d\n",offset,ftell(pIn),length);
					fread(retVal->memory + offset, sizeof(int8_t),length, pIn);
					break;
				case 0x7000:
					fread(&buf,sizeof(buf),1,pIn);
					printf("Found PC Address: %x at %x\n", buf, ftell(pIn));
					retVal->pc = buf;
					break;
				}
		}
		fclose(pIn);
		return retVal;
	}
	return NULL;
}


void setup(struct RCX8* state){
	if(state != NULL){
        reset(state);
	}
}

void dumpstate(struct RCX8* state){
	FILE * fout;
	fout = fopen("dump.out","wb");
	if(fout != NULL){
		printf("PC:%d\nACC:%d\n",state->pc,state->acc);
		fwrite(state->memory,sizeof(uint8_t),_MEMSIZE_, fout);
		fclose(fout);
	}
	
}
			
