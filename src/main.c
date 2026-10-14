#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include "program.h"
#include "loader.h"
#include "auxiliary.h"

int verbose = 0;
int step = 0;
FILE * logfile=0;

char kbi_abstract(){
    printf("kbi: ");
    return getchar();
}

void asc_abstract(char c){
    putchar(c);
}

void print_state(struct RCX8 * proc){
    printf("r0:%x r1:%x r2:%x r3:%x r4:%x\n"
           "r5:%x r6:%x r7:%x ac:%x cr:%x\n"
           "pc:%x sp:%x\n\n",
           proc->reg[0],proc->reg[1],proc->reg[2],proc->reg[3],
           proc->reg[4],proc->reg[5],proc->reg[6],proc->reg[7],
           proc->acc, proc->carry,proc->pc,proc->spointer);
}

void proc1(struct RCX8 * proc){
    uint8_t acc = 0;
    int iacc = 0;

    for(int i=0;i<256*16;i++){
        acc += proc->memory[i];
        iacc += proc->memory[i];
    }
    printf("Sum: 0x%x 0x%x\n", acc, iacc);
}

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
        //flags: v = verbose log, s = single-step / pause at jumps
        for(char * f = argv[1]; *f; f++){
            if(*f == 'v'){ verbose = true;}
            else if(*f == 's'){ step = true;}
            else{ printf("Unkown parameter %c\n", *f); }
        }
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

    if(verbose){
        proc1(proc);
    }

    if(verbose) printf("und go.\n");
    proc->run = 1;
    int c;

   do{
        struct OP op;
        op = read_op(proc);
        if(verbose) printf("%x %s\n",proc->pc,op2str(op));
        if(step){
            switch(op.opid){
            case _jun:
            case _jms:
            case _bbl:
                print_state(proc);
                getchar();
            }
        }
        *proc = execute_op(op,*proc);

        if(step && (proc->pc == 0x1000 || proc->pc == 0x1100)){
            print_state(proc);
            getchar();
        }
    }while(proc && proc->run);

    putchar('\n');
    print_state(proc);

    while(step && (c = getchar()) != EOF){
        struct OP op;
        op = read_op(proc);
        printf("%x %s",proc->pc,op2str(op));
        *proc = execute_op(op,*proc);
        if(c == 'd'){
            dumpstate(proc);
        }else if(c == 'p'){
            print_state(proc);
        }
    }

    return 0;
}

