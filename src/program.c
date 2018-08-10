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

void reset(struct RCX8* proc){
    memset(proc->reg,0,8);
    memset(proc->memory,0,65536);
    memset(proc->stack,0,256);
    proc->spointer = 0;
    proc->acc = 0;
    proc->pc = 0;
    proc->carry = 0;
    proc->run = 1;
}

void read_op(struct RCX8* proc)
{
    uint8_t op;
    op = proc->memory[proc->pc];

    if(op == 0xFD)
        dbgprintf(logfile,"found asc\n");

    uint8_t   hi = op >> 4;
    uint8_t lo = op & 0x0F;
    if      (hi == 0) nop(proc);
    else if(hi ==  1) jcn(proc,op);
    else if(hi ==  2) fimzer(proc,op);
    else if(hi ==  3) fin(proc,op);
    else if(hi ==  4) jun(proc,op);
    else if(hi ==  5) jms(proc,op);
    else if(hi ==  6) incino(proc,op);
    else if(hi ==  7) iszsto(proc,op);
    else if(hi ==  8) addxor(proc,op);
    else if(hi ==  9) substi(proc,op);
    else if(hi == 10) ld(proc,op);
    else if(hi == 11) xch(proc,op);
    else if(hi == 12) bbl(proc,op);
    else if(hi == 13) ldm(proc,op);
    else if(hi == 14) wmprdr(proc,op);
    else{
        if     (lo ==  0) clb(proc);
        else if(lo ==  1) clc(proc);
        else if(lo ==  2) iac(proc);
        else if(lo ==  3) cmc(proc);
        else if(lo ==  4) cma(proc);
        else if(lo ==  5) ral(proc);
        else if(lo ==  6) rar(proc);
        else if(lo ==  7) tcc(proc);
        else if(lo ==  8) dac(proc);
        else if(lo == 10) stc(proc);
        else if(lo == 11) daa(proc);
        else if(lo == 12) kbi(proc);
        else if(lo == 13) asc(proc);
        else if(lo == 15) hlt(proc);
    }
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

void fimzer(struct RCX8* proc,uint8_t in){
    if((in&1)==0)
        fim(proc,in);
    else
        zer(proc,in);
}

void incino(struct RCX8* proc,uint8_t in){
    if((in&1)==0)
        inc(proc,in);
    else
        ino(proc,in);
}

void iszsto(struct RCX8* proc,uint8_t in){
    if((in&1)==0)
        isz(proc,in);
    else
        sto(proc,in);
}

void addxor(struct RCX8* proc,uint8_t in){
    if((in&1)==0)
        add(proc,in);
    else
        x0r(proc,in);
}

void substi(struct RCX8* proc,uint8_t in){
    if((in&1)==0)
        sub(proc,in);
    else
        sti(proc,in);
}

void wmprdr(struct RCX8* proc,uint8_t in){
    if((in&1)==0)
        wmp(proc);
    else
        rdr(proc);
}

void jcn(struct RCX8* proc,uint8_t op)
{
    uint16_t addr;
    bool proceed;
    proceed = false;
    bool invert = true;

    dbgprintf(logfile, "jcn\n");

    if(op&0b1000){ //invert bit set?
        invert = false;
    }
    if(op&0b100) //check for acc = 0
    {
        if((proc->acc == 0) == invert)
            proceed = true;
    }
    if(op&0b10)
    {
        if((proc->carry == 1) == invert)
            proceed = true;
        else
            proceed = false;
    }
    if(op&0b1)
    {
        dbgprintf(logfile, "Check test signal... Not Implemented. Aborting.\n");
        //check for test signal
    }

    if(proceed)
    {
        proc->pc = fetch_addr(proc,proc->pc+1);
    }else{
        proc->pc += 3;
    }
}

void fin(struct RCX8* proc,uint8_t regno)
{
    uint16_t addr;
    addr = (proc->reg[1] << 8) | proc->reg[0];
    regno = get_regno(regno);

    proc->reg[regno] = proc->memory[addr];
    dbgprintf(logfile,"%#x fin R%d addr: %#x,%c\n",proc->pc,regno,addr,proc->memory[addr]);

    proc->pc++;
}

void fim(struct RCX8* proc,uint8_t regno)
{
    regno = get_regno(regno);
    proc->reg[regno]=proc->memory[proc->pc+1];
    dbgprintf(logfile, "%#x fim R%d %d\n",proc->pc,regno, proc->reg[regno]);
    proc->pc +=2;
}

void zer(struct RCX8* proc,uint8_t regno)
{
    regno = get_regno(regno);
    proc->reg[regno]=0;
    dbgprintf(logfile, "%#x zer R%d\n",proc->pc,regno);
    proc->pc++;
}

void ftc(struct RCX8* proc,uint8_t regno)
{
    regno = get_regno(regno);
    uint16_t addr=fetch_addr(proc,proc->pc+1);
    proc->reg[regno]=proc->memory[addr];
    //printf("addr:%u,memory:%u,pc:%u\n",addr,memory[addr],pc);
    dbgprintf(logfile, "ftc R%d\n",regno);
    proc->pc += 3;
}

void jun(struct RCX8* proc,uint8_t op)
{
    dbgprintf(logfile, "%#x jun %#x\n", proc->pc,fetch_addr(proc,proc->pc+1));
    proc->pc = fetch_addr(proc,proc->pc+1);
    dumpstate(proc);
}

void jms(struct RCX8* proc,uint8_t op)
{
    proc->spointer++;
    if(proc->spointer>15)
        proc->spointer = 0;
    proc->stack[proc->spointer] = proc->pc+3; //pc+3 ?
    dbgprintf(logfile, "jms\n");
    proc->pc = fetch_addr(proc,proc->pc+1);
}

void inc(struct RCX8* proc,uint8_t regno)
{
    uint8_t buffer;
    regno = get_regno(regno);
    buffer = proc->reg[regno];
    proc->reg[regno]++;
    //check for overflow
    if(proc->reg[regno]<buffer)
        proc->carry = true;
    else
        proc->carry = false;

    dbgprintf(logfile, "%#x inc R%d=%d C:%d\n", proc->pc, regno,proc->reg[regno],proc->carry);
    proc->pc++;
}

void ino(struct RCX8* proc,uint8_t regno)
{
    regno = get_regno(regno);
    dbgprintf(logfile, "%#x ino R%d=%d C:%d\n", proc->pc, regno,proc->reg[regno],proc->carry);
    if(proc->carry == true)
    {
        uint8_t buffer;
        regno = get_regno(regno);
        buffer = proc->reg[regno];
        proc->reg[regno]++;
        //check for overflow
        if(proc->reg[regno]<buffer){
            proc->carry = true;
        }else{
            proc->carry = false;
        }
    }
    proc->pc++;
}

void isz(struct RCX8* proc,uint8_t regno)
{
    regno = get_regno(regno);
    proc->reg[regno]++;
    dbgprintf(logfile, "%#x isz R%d=%d %#.4x\n",proc->pc, regno,proc->reg[regno], fetch_addr(proc,proc->pc+1));
    if(proc->reg[regno] != 0){
        proc->pc = fetch_addr(proc,proc->pc+1);
    }
    else proc->pc += 3;

}


void sto(struct RCX8* proc,uint8_t regno)
{
    regno = get_regno(regno);
    uint16_t a = fetch_addr(proc,proc->pc+1);
    dbgprintf(logfile, "%#x sto R%d uint16_t %#x\n",proc->pc,regno,a);
    proc->memory[a] = proc->reg[regno];
    proc->pc += 3;
}

void add(struct RCX8* proc,uint8_t regno)
{
    regno = get_regno(regno);
    uint8_t tmp,tmp2;
    dbgprintf(logfile, "%#x add R%d\n", proc->pc, regno);
    dbgprintf(logfile, "C:%d R%d + A%d = ",proc->carry, proc->reg[regno], proc->acc);
#if __GNUC__ > 4
    //R = A + R + C
    if(__builtin_add_overflow(proc->acc, proc->reg[regno],&tmp)){
        proc->carry = true;
    }else{
        proc->carry = false;
    }
    if(__builtin_sub_overflow(tmp, proc->carry, &tmp2))
        proc->carry |= true;

    proc->reg[regno] = tmp2;
#else
    add_of(proc->carry, proc->acc, proc->reg[regno]);
#endif
    dbgprintf(logfile, "%d\n", proc->reg[regno]);
    proc->pc++;
}

void x0r(struct RCX8* proc,uint8_t regno)
{
    regno = get_regno(regno);
    dbgprintf(logfile, "xor R%d\n",regno);
    proc->reg[regno] = proc->reg[regno]^(proc->acc);
    proc->pc++;
}

void sub(struct RCX8* proc,uint8_t regno)
{
    regno = get_regno(regno);
    uint8_t tmp,tmp2;
    dbgprintf(logfile, "sub R%d\n", regno);
#if __GNUC__ > 4
    //R = A - R - C
    if(__builtin_sub_overflow(proc->acc, proc->reg[regno],&tmp))
        proc->carry = true;
    if(__builtin_sub_overflow(tmp, proc->carry, &tmp2))
        proc->carry |= true;

    proc->reg[regno] = tmp2;    
#endif
    proc->pc++;
}

void sti(struct RCX8* proc,uint8_t regno)
{
    regno = get_regno(regno);

    uint16_t addr;
    addr = (proc->reg[1] <<8) | proc->reg[0];
    dbgprintf(logfile, "sti R%d %#x\n",regno,addr);
    proc->memory[addr] = proc->reg[regno];

    proc->pc++;
}

void ld(struct RCX8* proc,uint8_t regno)
{
    regno = get_regno(regno);
    proc->acc = proc->reg[regno];
    dbgprintf(logfile, "%#x ld R%d\n", proc->pc, regno);
    proc->pc++;
}

void xch(struct RCX8* proc,uint8_t regno)
{
    dbgprintf(logfile, "xch R\n");
    regno = get_regno(regno);
    uint8_t swap;
    swap = proc->acc;
    proc->acc = proc->reg[regno];
    proc->reg[regno] = swap;
    proc->pc++;
}

void bbl(struct RCX8* proc,uint8_t op)
{
    dbgprintf(logfile, "bbl %#x\n",op&0xf);
    proc->pc = proc->stack[proc->spointer];
    proc->spointer--; //no need to check for underflow
    proc->acc = op & 0xF;
}

void ldm(struct RCX8* proc,uint8_t data)
{
    dbgprintf(logfile, "ldm %d\n", data&0xf);
    proc->acc = data & 0x0F; 
    proc->pc++;
}

void wmp(struct RCX8* proc)
{
    //TODO
    dbgprintf(logfile, "wmp\n");
    proc->pc++;
}

void rdr(struct RCX8* proc)
{
    dbgprintf(logfile, "rdr\n");
    //TODO:
    proc->pc++;
}

void kbi(struct RCX8* proc)
{
    dbgprintf(logfile, "kbi\n");
    proc->acc = getchar();
    proc->pc++;
}

void asc(struct RCX8* proc)
{
    dbgprintf(logfile, "%#x asc\n",proc->pc);
    printf("%c",proc->acc);
    proc->pc++;
}

void clb(struct RCX8* proc)
{
    proc->acc = 0;
    proc->carry = 0;
    dbgprintf(logfile, "%#x clb\n", proc->pc);
    proc->pc++;
}

void clc(struct RCX8* proc) //TODO: mit asc getauscht, weil da ganz offensichtlich n fehler ist...
{
    proc->carry = 0;
    dbgprintf(logfile, "%#x clc\n",proc->pc);
    proc->pc++;
    
}

void iac(struct RCX8* proc)
{
    dbgprintf(logfile, "iac\n");
    uint8_t tmp = proc->acc;
    if(tmp < ++proc->acc)
        proc->carry = true;
    proc->pc++;
}

void cmc(struct RCX8* proc)
{
    dbgprintf(logfile, "cmc\n");
    proc->carry = (proc->carry==false)?true:false;
    proc->pc++;
}

void cma(struct RCX8* proc)
{
    dbgprintf(logfile, "cma\n");
    proc->acc = ~proc->acc;
    proc->pc++;
}

void ral(struct RCX8* proc)
{
    dbgprintf(logfile, "ral\n");
    uint8_t lsb = proc->carry;
    proc->carry = proc->acc & 0x80; // carry = msb
    proc->acc = (proc->acc << 1) & lsb;

    proc->pc++;
}

void rar(struct RCX8* proc)
{
    dbgprintf(logfile, "rar\n");
    uint8_t msb = proc->carry;
    proc->carry = proc->acc & 0x01; // carry = lsb
    proc->acc = (proc->acc >> 1) & (msb << 7);

    proc->pc++;
}

void tcc(struct RCX8* proc)
{
    dbgprintf(logfile, "tcc\n");
    proc->acc = proc->carry;
    proc->pc++;
}

void dac(struct RCX8* proc)
{
    uint8_t tmp = proc->acc;
    dbgprintf(logfile, "dac\n");
    if(tmp > --proc->acc)
        proc->carry = true;
    proc->pc++;
}

void stc(struct RCX8* proc)
{
    proc->carry = true;
    dbgprintf(logfile, "stc\n");
    proc->pc++;
}

void daa(struct RCX8* proc) //TODO BCD adjust
{
    dbgprintf(logfile, "daa\n");
    proc->pc++;
}

void hlt(struct RCX8* proc)
{
    proc->run = 0;
    dbgprintf(logfile, "hlt\n");
}

