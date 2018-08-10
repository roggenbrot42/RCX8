#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include "program.h"
#include "loader.h"
#include "auxiliary.h"

int verbose = 0;
FILE * logfile=0;

int main(int argc, char * argv[])
{
    struct RCX8* proc=0;
    
    
    if(argc < 2){
        printf("Insufficient arguments\n");
        return -1;
    }
    if(argc == 2){
        proc=bootstrap(argv[1]);
    }
    if(argc == 3){
        if(*argv[1] == 'v'){ verbose = true;}
        else{ printf("Unkown parameter\n"); }
        proc=bootstrap(argv[2]);
    }

    if(!proc){
        printf("Couldn't load proc file\n");
        return -1;
    }

    if(verbose){
        logfile = fopen("out.log","w");
        if(logfile==0){
            verbose = 0;
            fprintf(stderr,"Error opening log file...\n");
        }
    }

    printf("und go.\n");

    while(proc && proc->run)
    {
        read_op(proc);
    }


    return 0;
}

