#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include "program.h"
#include "loader.h"
#include "auxiliary.h"

#define __HALF_MAX_SIGNED(type) ((type)1 << (sizeof(type)*8-2))
#define __MAX_SIGNED(type) (__HALF_MAX_SIGNED(type) - 1 + __HALF_MAX_SIGNED(type))
#define __MIN_SIGNED(type) (-1 - __MAX_SIGNED(type))

#define __MIN(type) ((type)-1 < 1?__MIN_SIGNED(type):(type)0)
#define __MAX(type) ((type)~__MIN(type))

#define assign(dest,src) ({ \
        __typeof__(src) __x=(src); \
        __typeof__(dest) __y=__x; \
        (__x==__y && ((__x<1) == (__y<1))?(void)((dest)=__y),0:1); \
        })
#define add_of(c,a,b) ({ \
        __typeof__(a) __a=a; \
        __typeof__(b) __b=b; \
        (__b)<1 ? \
        ((__MIN(__typeof__(c))-(__b)<=(__a))?assign(c,__a+__b):1) : \
        ((__MAX(__typeof__(c))-(__b)>=(__a))?assign(c,__a+__b):1); \
        })
#define sub_of(c,a,b) ({ \
        __typeof__(a) __a=a; \
        __typeof__(b) __b=b; \
        (__b)<1 ? \
        ((__MAX(__typeof__(c))+(__b)>=(__a))?assign(c,__a-__b):1) : \
        ((__MIN(__typeof__(c))+(__b)<=(__a))?assign(c,__a-__b):1); \
        })

#define dbgprintf(...) ; 

const char * const op_name[] = {
   "nop",
   "jcn",
   "fin",
   "fim",
   "zer",
   "ftc",
   "jun",
   "jms",
   "inc",
   "ino",
   "isz",
   "sto",
   "add",
   "xor",
   "sub",
   "sti",
   "ld",
   "xch",
   "bbl",
   "ldm",
   "wmp",
   "rdr",
   "kbi",
   "asc",
   "clb",
   "clc",
   "iac",
   "cmc",
   "cma",
   "ral",
   "rar",
   "tcc",
   "dac",
   "stc",
   "daa",
   "hlt"
};

void reset(struct RCX8* proc){
    memset(proc->reg,0,sizeof(uint8_t)*_REGISTERCOUNT_);
    memset(proc->memory,0,sizeof(uint8_t)*_MEMSIZE_);
    memset(proc->stack,0,sizeof(uint8_t)*_CALLSTACKSIZE_);
    memset(proc->port,0,sizeof(uint8_t)*_PORTNUM_);

    proc->spointer = 0;
    proc->acc = 0;
    proc->pc = 0;
    proc->carry = 0;
    proc->run = 0;
    proc->test = 0;
    proc->buf_opstr = 0;
}

struct OP read_op(struct RCX8* proc)
{
    uint8_t opcode;
    struct OP op;
    opcode = proc->memory[proc->pc];

    uint8_t hi = opcode >> 4;
    uint8_t lo = opcode & 0x0F;
    uint8_t loo = lo & 1;

    op.opr = hi;
    op.opa = lo;
    op.arg[0] = 0;
    op.arg[1] = 0;
    op.arg_t[0] = arg_none;
    op.arg_t[1] = arg_none;




    if      (hi == 0) op.opid = _nop;
    else if(hi ==  1) {
        op.opid = _jcn;
        op.arg[0] = lo;
        op.arg_t[0] = arg_cnd;
        op.arg[1] = proc->memory[proc->pc+1];
        op.arg_t[1] = arg_addr;
    }
    else if(hi ==  2 && loo==0){
        op.opid = _fim;
        op.arg[0] = lo>>1;
        op.arg_t[0] = arg_reg;
        op.arg[1] = proc->memory[proc->pc+1];
        op.arg_t[1] = arg_dirct;
    }
    else if(hi ==  2 && loo==1){
        op.opid = _zer;
        op.arg[0] = lo>>1;
        op.arg_t[0] = arg_reg;
    }
    else if(hi ==  3 && loo==0){
        op.opid = _fin;
        op.arg[0] = lo>>1;
        op.arg_t[0] = arg_reg;
    }
    else if(hi == 3 && loo==1){
        op.opid = _ftc;
        op.arg[0] = lo>>1;
        op.arg_t[0]=arg_reg;
        op.arg[1] = fetch_addr(proc,proc->pc+1);
        op.arg_t[1] = arg_ptr;
    }
    else if(hi ==  4){
        op.opid = _jun;
        op.arg[0] = fetch_addr(proc,proc->pc+1);
        op.arg_t[0] = arg_addr;
    }
    else if(hi ==  5){
        op.opid = _jms;
        op.arg[0] = fetch_addr(proc,proc->pc+1);
        op.arg_t[0] = arg_addr;
    }
    else if(hi ==  6 && loo == 0){
        op.opid = _inc;
        op.arg[0] = lo>>1;
        op.arg_t[0] = arg_reg;
    }
    else if(hi ==  6 && loo == 1){
        op.opid = _ino;
        op.arg[0] = lo>>1;
        op.arg_t[0] = arg_reg;
    }
    else if(hi ==  7 && loo == 0){
        op.opid = _isz;
        op.arg[0] = lo>>1;
        op.arg_t[0] = arg_reg;
        op.arg[1] = fetch_addr(proc, proc->pc+1);
        op.arg_t[1] = arg_addr;
    }
    else if(hi ==  7 && loo == 1){
        op.opid = _sto;
        op.arg[0] = lo>>1;
        op.arg_t[0] = arg_reg;
        op.arg[1] = fetch_addr(proc,proc->pc+1);
        op.arg_t[1] = arg_ptr;
    }
    else if(hi ==  8 && loo==0){
        op.opid = _add;
        op.arg[0] = lo>>1;
        op.arg_t[0] = arg_reg;
    }
    else if(hi == 8 && loo==1){
        op.opid = _x0r;
        op.arg[0] = lo>>1;
        op.arg_t[0] = arg_reg;
    }
    else if(hi ==  9 && loo==0){
        op.opid = _sub;
        op.arg[0] = lo>>1;
        op.arg_t[0] = arg_reg;
    }
    else if(hi ==  9 && loo==1){
        op.opid = _sti;
        op.arg[0] = lo>>1;
        op.arg_t[0] = arg_reg;
    }
    else if(hi == 10){
        op.opid = _ld;
        op.arg[0] = lo>>1;
        op.arg_t[0] = arg_reg;
    }
    else if(hi == 11){
        op.opid = _xch;
        op.arg[0] = lo>>1;
        op.arg_t[0] = arg_reg;
    }
    else if(hi == 12){
        op.opid = _bbl;
        op.arg[0] = lo;
        op.arg_t[0] = arg_dirct;
    }
    else if(hi == 13){
        op.opid = _ldm;
        op.arg[0] = lo;
        op.arg_t[0] = arg_dirct;
    }
    else if(hi == 14 && loo == 1){
        op.opid = _wmp;
        op.arg[0] = proc->memory[proc->pc+1];
        op.arg_t[0] = arg_port;
    }
    else if(hi == 14 && loo == 0){
        op.opid = _rdr;
        op.arg[0] = proc->memory[proc->pc+1];
        op.arg_t[0] = arg_port;
    }
    else{
        if     (lo ==  0) op.opid = _clb;
        else if(lo ==  1) op.opid = _clc;
        else if(lo ==  2) op.opid = _iac;
        else if(lo ==  3) op.opid = _cmc;
        else if(lo ==  4) op.opid = _cma;
        else if(lo ==  5) op.opid = _ral;
        else if(lo ==  6) op.opid = _rar;
        else if(lo ==  7) op.opid = _tcc;
        else if(lo ==  8) op.opid = _dac;
        else if(lo == 10) op.opid = _stc;
        else if(lo == 11) op.opid = _daa;
        else if(lo == 12) op.opid = _kbi;
        else if(lo == 13) op.opid = _asc;
        else if(lo == 15) op.opid = _hlt;
    }

    return op;
}

struct RCX8 execute_op(struct OP op, struct RCX8 proc){

    switch(op.opid){
    case _nop:
        nop(&proc);
        break;
    case _jcn:
        jcn(&proc,op.arg[0],op.arg[1]);
        break;
    case _fim:
        fim(&proc,op.arg[0],op.arg[1]);
        break;
    case _zer:
        zer(&proc,op.arg[0]);
        break;
    case _fin:
        fin(&proc,op.arg[0]);
        break;
    case _ftc:
        ftc(&proc,op.arg[0],op.arg[1]);
        break;
    case _jun:
        jun(&proc,op.arg[0]);
        break;
    case _jms:
        jms(&proc, op.arg[0]);
        break;
    case _inc:
        inc(&proc, op.arg[0]);
        break;
    case _ino:
        ino(&proc, op.arg[0]);
        break;
    case _isz:
        isz(&proc,op.arg[0],op.arg[1]);
        break;
    case _sto:
        sto(&proc,op.arg[0],op.arg[1]);
        break;
    case _add:
        add(&proc, op.arg[0]);
        break;
    case _x0r:
        x0r(&proc, op.arg[0]);
        break;
    case _sub:
        sub(&proc, op.arg[0]);
        break;
    case _sti:
        sti(&proc,op.arg[0]);
        break;
    case _ld:
        ld(&proc,op.arg[0]);
        break;
    case _xch:
        xch(&proc,op.arg[0]);
        break;
    case _bbl:
        bbl(&proc,op.arg[0]);
        break;
    case _ldm:
        ldm(&proc,op.arg[0]);
        break;
    case _wmp:
        wmp(&proc,op.arg[0]);
        break;
    case _rdr:
        rdr(&proc,op.arg[0]);
        break;
    case _kbi:
        kbi(&proc);
        break;
    case _asc:
        asc(&proc);
        break;
    case _clb:
        clb(&proc);
        break;
    case _clc:
        clc(&proc);
        break;
    case _iac:
        iac(&proc);
        break;
    case _cmc:
        cmc(&proc);
        break;
    case _cma:
        cma(&proc);
        break;
    case _ral:
        ral(&proc);
        break;
    case _rar:
        rar(&proc);
        break;
    case _tcc:
        tcc(&proc);
        break;
    case _dac:
        dac(&proc);
        break;
    case _stc:
        stc(&proc);
        break;
    case _daa:
        daa(&proc);
        break;
    case _hlt:
        hlt(&proc);
        break;
    default:
        break;
    }

    return proc;
}

char * op2str(struct OP op){
    static char opstr[31];
    const char arg_name[num_argst] = {'\0','R','D','C','$','M','P'};
    uint8_t len = 0;
    memset(opstr,'\0',31);

    len = sprintf(opstr,"%s",op_name[op.opid]);

    for(int i=0;i<2;i++){
        if(op.arg_t[i] == arg_none) break;
        if(i == 1) len += sprintf(opstr+len,",");
        switch(op.arg_t[i]){
        case arg_reg:
        case arg_addr:
        case arg_cnd:
        case arg_port:
            len += sprintf(opstr+len, " %c0x%x",arg_name[op.arg_t[i]],op.arg[i]);
            break;
        case arg_dirct:
            len += sprintf(opstr+len, " %u",op.arg[i]);
            break;
        case arg_ptr:
            len += sprintf(opstr+len, " [0x%x]",op.arg[i]);
            break;
        }
    }

 /*   switch(op.opid){
    case _ral:
    case _rar:
        len += sprintf(opstr+len, " acc (%x) = (%x)",op.arg[0],op.arg[0]<<1);
        break;
    default:
        break;
    } */

    return opstr;
}

inline uint8_t get_regno(uint8_t in)
{
    return (uint8_t) (in >> 1) & 7;
}

inline uint16_t fetch_addr(struct RCX8* proc,uint16_t in)
{

    uint16_t addr=0;
    addr = (proc->memory[in+1] << 8) | proc->memory[in];

    return addr;
}

void nop(struct RCX8* proc)
{
    proc->pc++;
}

void jcn(struct RCX8* proc,uint8_t cond,uint16_t addr)
{
    bool proceed;
    proceed = false;
    bool invert = true;

    if(cond&0b1000){ //invert bit set?
        invert = false;
    }
    if(cond&0b100) //check for acc = 0
    {
        if((proc->acc == 0) == invert)
            proceed = true;
    }
    if(cond&0b10)
    {
        if((proc->carry == 1) == invert)
            proceed = true;
        else
            proceed = false;
    }
    if(cond&0b1)
    {
        if((proc->test == 1) == invert){
            proceed = true;
        }
        else{
            proceed = false;
        }
    }

    if(proceed)
    {
        proc->pc = addr;
    }else{
        proc->pc += 3;
    }
}

void fim(struct RCX8* proc,uint8_t regno,uint8_t dirct)
{
    proc->reg[regno]=dirct;
    proc->pc +=2;
}

void zer(struct RCX8* proc,uint8_t regno)
{
    proc->reg[regno]=0;
    proc->pc++;
}

void fin(struct RCX8* proc,uint8_t regno)
{
    uint16_t addr;
    addr = (proc->reg[1] << 8) | proc->reg[0];

    proc->reg[regno] = proc->memory[addr];
    proc->pc++;
}

void ftc(struct RCX8* proc,uint8_t regno,uint16_t addr)
{
    proc->reg[regno]=proc->memory[addr];
    proc->pc += 3;
}

void jun(struct RCX8* proc,uint16_t addr)
{
    proc->pc = addr;
}

void jms(struct RCX8* proc,uint16_t addr)
{
    proc->spointer++;
    if(proc->spointer>15)
        proc->spointer = 0;
    proc->stack[proc->spointer] = proc->pc+3; //pc+3 ?
    proc->pc = addr;
}

void inc(struct RCX8* proc,uint8_t regno)
{
    uint8_t buffer;
    buffer = proc->reg[regno];
    proc->reg[regno]++;
    //check for overflow
    if(proc->reg[regno]<buffer)
        proc->carry = true;
    else
        proc->carry = false;

    proc->pc++;
}

void ino(struct RCX8* proc,uint8_t regno)
{
    if(proc->carry == true)
    {
        uint8_t buffer;
        buffer = proc->reg[regno];
        proc->reg[regno]++;
        //check for overflow
        if(proc->reg[regno]<buffer){
            proc->carry = true;
        }else{
            proc->carry = false;
        }
    }else{
        proc->carry = false;
    }
    proc->pc++;
}

void isz(struct RCX8* proc,uint8_t regno,uint16_t addr)
{
    proc->reg[regno]++;
    if(proc->reg[regno] != 0){
        proc->pc = addr;
    }
    else proc->pc += 3;
}


void sto(struct RCX8* proc,uint8_t regno,uint16_t addr)
{
    proc->memory[addr] = proc->reg[regno];
    proc->pc += 3;
}

void add(struct RCX8* proc,uint8_t regno)
{
    uint8_t tmp,tmp2;
#if __GNUC__ > 4
    //R = A + R + C
    if(__builtin_add_overflow(proc->acc, proc->reg[regno],&tmp)){
        proc->carry = true;
    }else{
        proc->carry = false;
    }
    if(__builtin_add_overflow(tmp, proc->carry, &tmp2))
        proc->carry |= true;

    proc->reg[regno] = tmp2;
#else
    add_of(proc->carry, proc->acc, proc->reg[regno]);
#endif
    proc->pc++;
}

void x0r(struct RCX8* proc,uint8_t regno)
{
    proc->reg[regno] = proc->reg[regno]^(proc->acc);
    proc->pc++;
}

void sub(struct RCX8* proc,uint8_t regno)
{
    uint8_t tmp,tmp2;
#if __GNUC__ > 4
    //R = A - R - C
    if(__builtin_sub_overflow(proc->acc, proc->reg[regno],&tmp)){
        proc->carry = true;
    }else{
        proc->carry = false;
    }
    if(__builtin_sub_overflow(tmp, proc->carry, &tmp2))
        proc->carry |= true;

    proc->reg[regno] = tmp2;    
#endif
    proc->pc++;
}

void sti(struct RCX8* proc,uint8_t regno)
{
    uint16_t addr;
    addr = (proc->reg[1] <<8) | proc->reg[0];
    proc->memory[addr] = proc->reg[regno];

    proc->pc++;
}

void ld(struct RCX8* proc,uint8_t regno)
{
    proc->acc = proc->reg[regno];
    proc->pc++;
}

void xch(struct RCX8* proc,uint8_t regno)
{
    uint8_t swap;
    swap = proc->acc;
    proc->acc = proc->reg[regno];
    proc->reg[regno] = swap;
    proc->pc++;
}

void bbl(struct RCX8* proc,uint8_t drct)
{
    proc->pc = proc->stack[proc->spointer];
    proc->spointer--; //no need to check for underflow
    proc->acc = drct;
}

void ldm(struct RCX8* proc,uint8_t data)
{
    proc->acc = data;
    proc->pc++;
}

void wmp(struct RCX8* proc,uint8_t port)
{
    proc->port[port] = proc->acc;
    proc->pc++;
}

void rdr(struct RCX8* proc,uint8_t port)
{
    proc->acc = proc->port[port];
    proc->pc++;
}

void kbi(struct RCX8* proc)
{
    proc->acc = kbi_abstract();
    proc->pc++;
}

void asc(struct RCX8* proc)
{
    asc_abstract(proc->acc);
    proc->pc++;
}

void clb(struct RCX8* proc)
{
    proc->acc = 0;
    proc->carry = 0;
    proc->pc++;
}

void clc(struct RCX8* proc){
    proc->carry = 0;
    proc->pc++;
}

void iac(struct RCX8* proc)
{
    uint8_t tmp = proc->acc;
    if(tmp < ++proc->acc)
        proc->carry = true;
    else
        proc->carry = false;
    proc->pc++;
}

void cmc(struct RCX8* proc)
{
    proc->carry = (proc->carry==0)?1:0;
    proc->pc++;
}

void cma(struct RCX8* proc)
{
    proc->acc = ~proc->acc;
    proc->pc++;
}

void ral(struct RCX8* proc)
{
    uint8_t lsb = proc->carry;
    proc->carry = proc->acc & (0x80 >> 7); // carry = msb
    proc->acc = (proc->acc << 1) | lsb;

    printf("ral: %x %x\n", proc->carry, proc->acc);

    proc->pc++;
}

void rar(struct RCX8* proc)
{
    uint8_t msb = proc->carry;
    proc->carry = proc->acc & 0x01; // carry = lsb
    proc->acc = (proc->acc >> 1) | (msb << 7);

    proc->pc++;
}

void tcc(struct RCX8* proc)
{
    proc->acc = proc->carry;
    proc->carry = 0;
    proc->pc++;
}

void dac(struct RCX8* proc)
{
    uint8_t tmp = proc->acc;
    dbgprintf(logfile, "dac\n");
    if(tmp < --proc->acc)
        proc->carry = true;
    else
        proc->carry = false;
    proc->pc++;
}

void stc(struct RCX8* proc)
{
    proc->carry = true;
    proc->pc++;
}

void daa(struct RCX8* proc) //TODO BCD adjust
{
    printf("daa =========\n");
    proc->pc++;
}

void hlt(struct RCX8* proc)
{
    proc->run = 0;
}

