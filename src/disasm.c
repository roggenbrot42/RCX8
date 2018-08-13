#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

#include "loader.h"
#include "program.h"

char kbi_abstract(){
    return getchar();
}

void asc_abstract(char c){
    putchar(c);
}

int verbose = 0;

int main(int argc, char ** argv){

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

    struct OP op;
    for(int i=0;i<868;i++){
        op = read_op(proc);
        printf("%x %s \n",proc->pc,op2str(op));

        switch(op.opid){
        case _jcn:
        case _ftc:
        case _jun:
        case _jms:
        case _isz:
        case _sto:
            proc->pc += 3;
            break;
        case _fim:
        case _wmp:
        case _rdr:
            proc->pc += 2;
            break;
        default:
            proc->pc++;
        }
    }

}
