#include <string.h>
#include <stdio.h>
#include <stdbool.h>
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


int main(int argc, char * argv[])
{
    mapopc();
    
    if(argc > 1){
	    proc=bootstrap(argv[1]);
	}
	else{
		printf("Insufficient arguments\n");
		return 1;
	}

    dumpstate(proc);

	printf("und go.\n");

    /*memory[0]   = 0b00100010; //FIM R1 8
    memory[1]   = 0b00001000; // => R1.R0 = 2048
    memory[3]   = 0b00110100; //fin R2
    memory[4]   = 0b10100100; //ld acc r2
    memory[5]   = 0b00000000; //nop
    memory[6]   = 0b00011100; //jump nif acc=0
    memory[7]   = 0b00000000;
    memory[8]   = 0b00001100; //to 10
    memory[9]   = 0b01000000; //jun
    memory[10]  = 0; // to
    memory[11]  = 19; // 19
    memory[12]  = 0b10100100; // ld R2
    memory[13]  = 0b11111101; // asc
    memory[14]  = 0b01100000; // inc R0
    memory[15]  = 0b01100011; //ino R1
    memory[16]  = 0b01000000; //jun
    memory[17]  = 0;
    memory[18]  = 3; //to 3
    //memory[19]  = 0b11111100; //kbi
    memory[19]  = 255;


    memory[2048] = 'D'; //0x9
    memory[2049] = 'a';
    memory[2050] = 's';
    memory[2051] = ' ';
    memory[2052] = 'i';
    memory[2053] = 's';
    memory[2054] = 't';
    memory[2055] = ' ';
    memory[2056] = 'e';
    memory[2057] = 'i';
    memory[2058] = 'n';
    memory[2059] = ' ';
    memory[2060] = 't';
    memory[2061] = 'e';
    memory[2062] = 's';
    memory[2063] = 't';
    memory[2064] = '.';
    memory[2065] = '\n';
    memory[2066] = 'W';
    memory[2067] = 'e';
    memory[2068] = 'r';
    memory[2069] = ' ';
    memory[2070] = 'w';
    memory[2071] = 'i';
    memory[2072] = 'l';
    memory[2073] = 'l';
    memory[2074] = ' ';
    memory[2075] = 'S';
    memory[2076] = 'u';
    memory[2077] = 'p';
    memory[2078] = 'p';
    memory[2079] = 'e';
    memory[2080] = '?';
    memory[2081] = '\n';
    memory[2082] = '\0';
    */


while(proc->run)
{
    readOp();
}


    return 0;
}

void mapopc() {
    oprmap[0] = &nop;
    oprmap[1] = &jcn;
    oprmap[2] = &fimzer;
    oprmap[3] = &fin;
    oprmap[4] = &jun;
    oprmap[5] = &jms;
    oprmap[6] = &incino;
    oprmap[7] = &iszsto;
    oprmap[8] = &addxor;
    oprmap[9] = &substi;
    oprmap[10] = &ld;
    oprmap[11] = &xch;
    oprmap[12] = &bbl;
    oprmap[13] = &ldm;
    oprmap[14] = &wmprdr;
    oprmap[15] = &opa;
    
    opamap[0] = &clb;
    opamap[1] = &clc;
    opamap[2] = &iac;
    opamap[3] = &cmc;
    opamap[4] = &cma;
    opamap[5] = &ral;
    opamap[6] = &rar;
    opamap[7] = &tcc;
    opamap[8] = &dac;
    opamap[9] = NULL;
    opamap[10] = &stc;
    opamap[11] = &daa;
    opamap[12] = &kbi;
    opamap[13] = &asc;
    opamap[14] = NULL;
    opamap[15] = &hlt;
}

void reset(){
    memset(proc->reg,0,8);
    memset(proc->memory,0,65536);
    memset(proc->stack,0,256);
    proc->spointer = 0;
    proc->acc = 0;
    proc->pc = 0;
    proc->carry = 0;
    proc->run = 1;
}

void readOp()
{
    UBYTE op;
    op = proc->memory[proc->pc];
    
    if(op == 0xFD)
    	printf("found asc\n");
    
 //   printf("%#x %#x\n",op, proc->pc);
    
    oprmap[(op>>4)](op);
}

inline REGNO getRegno(BYTE in)
{
    return (REGNO) (in >> 1) & 7;
}

inline ADDR fetchADDR(ADDR in) //TODO: bug?
{
	
    ADDR addr=0;
    addr = (proc->memory[in+1] << 8) | proc->memory[in];
//    addr = (ADDR*)(proc->memory + in);
    
    return addr;
}

void fimzer(UBYTE in){
    if((in&1)==0)
        fim(in);
    else
        zer(in);
}

void incino(UBYTE in){
    if((in&1)==0)
        inc(in);
    else
        ino(in);
}

void iszsto(UBYTE in){
    if((in&1)==0)
        isz(in);
    else
        sto(in);
}

void addxor(UBYTE in){
    if((in&1)==0)
        add(in);
    else
        xor(in);
}

void substi(UBYTE in){
    if((in&1)==0)
        sub(in);
    else
        sti(in);
}

void wmprdr(UBYTE in){
    if((in&1)==0)
        wmp(in);
    else
        rdr(in);
}

void opa(UBYTE in){
    opamap[in&0xF]();
}

void nop(UBYTE op)
{
	proc->pc++;
	//proc->run = 0;
	fprintf(stderr, "nop pc: %x\n",proc->pc);

}

void jcn(UBYTE op)
{
    ADDR addr;
    bool proceed;
    proceed = false;
    bool invert = true;

	fprintf(stderr, "jcn\n");

    if(op&0b1000) //invert bit set?
        invert = false;
    if(op&4) //check for acc = 0
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
        //check for test signal
    }

    if(proceed)
    {
        proc->pc = fetchADDR(proc->pc+1);
    }
    if(!proceed)
    {
        proc->pc += 3;
    }
}

void fin(REGNO regno)
{
    ADDR addr;
    addr = (proc->reg[1] << 8) | proc->reg[0];
    regno = getRegno(regno);
   	fprintf(stderr,"%#x fin R%d addr: %#x\n",proc->pc,regno,addr);

    proc->reg[regno] = proc->memory[addr];

    proc->pc++;
}

void fim(REGNO regno)
{
    regno = getRegno(regno);
    proc->reg[regno]=proc->memory[proc->pc+1];
    fprintf(stderr, "%#x fim R%d %d\n",proc->pc,regno, proc->reg[regno]);
    proc->pc +=2;
}

void zer(REGNO regno)
{
    regno = getRegno(regno);
	proc->reg[regno]=0;
	fprintf(stderr, "%#x zer R%d\n",proc->pc,regno);
	proc->pc++;
}

void ftc(REGNO regno)
{
    regno = getRegno(regno);
	ADDR addr=fetchADDR(proc->pc+1);
	proc->reg[regno]=proc->memory[addr];
	//printf("addr:%u,memory:%u,pc:%u\n",addr,memory[addr],pc);
	fprintf(stderr, "ftc R%d\n",regno);
	proc->pc += 3;
}

void jun(UBYTE op)
{
    fprintf(stderr, "%#x jun %#x\n", proc->pc,fetchADDR(proc->pc+1));
    proc->pc = fetchADDR(proc->pc+1);
    dumpstate(proc);
}

void jms(UBYTE op)
{
    proc->spointer++;
    if(proc->spointer>15)
        proc->spointer = 0;
    proc->stack[proc->spointer] = proc->pc+3; //pc+3 ?
	fprintf(stderr, "jms\n");
    proc->pc = fetchADDR(proc->pc+1);
}

void inc(REGNO regno)
{
    UBYTE buffer;
    regno = getRegno(regno);
    buffer = proc->reg[regno];
	proc->reg[regno]++;
	//check for overflow
	if(proc->reg[regno]<buffer)
        proc->carry = true;
        
    fprintf(stderr, "%#x inc R%d\n", proc->pc, regno);
	proc->pc++;
}

void ino(REGNO regno)
{
    regno = getRegno(regno);
	fprintf(stderr, "%#x ino R%d\n", proc->pc, regno);
    if(proc->carry == true)
    {
        UBYTE buffer;
    	regno = getRegno(regno);
    	buffer = proc->reg[regno];
		proc->reg[regno]++;
		//check for overflow
		if(proc->reg[regno]<buffer)
        	proc->carry = true;
    }
    proc->pc++;
}

void isz(REGNO regno)
{
    regno = getRegno(regno);
    proc->reg[regno]++;
    fprintf(stderr, "%#x isz R%d %#.4x\n",proc->pc, regno, fetchADDR(proc->pc+1));
    if(proc->reg[regno] != 0){
        proc->pc = fetchADDR(proc->pc+1);
    }
    else proc->pc += 3;
    
}
    

void sto(REGNO regno)
{
    regno = getRegno(regno);
    ADDR a = fetchADDR(proc->pc+1);
    fprintf(stderr, "%#x sto R%d ADDR %#x\n",proc->pc,regno,a);
    proc->memory[a] = proc->reg[regno];
    proc->pc += 3;
}

void add(REGNO regno)
{
    regno = getRegno(regno);
	UBYTE tmp,tmp2;
	fprintf(stderr, "%#x add R%d\n", proc->pc, regno);
	#if __GNUC__ > 4
    //R = A + R + C
    if(__builtin_add_overflow(proc->acc, proc->reg[regno],&tmp))
    	proc->carry = true;
    if(__builtin_sub_overflow(tmp, proc->carry, &tmp2)
    	proc->carry |= true;

	proc->reg[regno] = tmp2;
	#else
		add_of(proc->carry, proc->acc, proc->reg[regno]);
	#endif
	proc->pc++;
}

void xor(REGNO regno)
{
    regno = getRegno(regno);
	fprintf(stderr, "xor R%d\n",regno);
    proc->reg[regno] = proc->reg[regno]^(proc->acc);
    proc->pc++;
}

void sub(REGNO regno)
{
    regno = getRegno(regno);
    UBYTE tmp,tmp2;
    fprintf(stderr, "sub R%d\n", regno);
	#if __GNUC__ > 4
    //R = A - R - C
    if(__builtin_sub_overflow(proc->acc, proc->reg[regno],&tmp))
    	proc->carry = true;
    if(__builtin_sub_overflow(tmp, proc->carry, &tmp2)
    	proc->carry |= true;

	proc->reg[regno] = tmp2;    
	#endif
	proc->pc++;
}

void sti(REGNO regno)
{
    regno = getRegno(regno);

    ADDR addr;
    addr = (proc->reg[1] <<8) | proc->reg[0];
	fprintf(stderr, "sti R%d %#x\n",regno,addr);
    proc->memory[addr] = proc->reg[regno];

    proc->pc++;
}

void ld(REGNO regno)
{
    regno = getRegno(regno);
	proc->acc = proc->reg[regno];
	fprintf(stderr, "%#x ld R%d\n", proc->pc, regno);
	proc->pc++;
}

void xch(REGNO regno)
{
	fprintf(stderr, "xch R\n");
    regno = getRegno(regno);
    UBYTE swap;
    swap = proc->acc;
    proc->acc = proc->reg[regno];
    proc->reg[regno] = swap;
    proc->pc++;
}

void bbl(UBYTE op)
{
	fprintf(stderr, "bbl %#x\n",op&0xf);
    proc->pc = proc->stack[proc->spointer];
    proc->spointer--; //no need to check for underflow
    proc->acc = op & 0xF;
}

void ldm(UBYTE data)
{
	fprintf(stderr, "ldm %d\n", data&0xf);
    proc->acc = data & 0x0F; 
    proc->pc++;
}

void wmp()
{
	//TODO
	fprintf(stderr, "wmp\n");
    proc->pc++;
}

void rdr()
{
	fprintf(stderr, "rdr\n");
	//TODO:
    proc->pc++;
}

void kbi()
{
	fprintf(stderr, "kbi\n");
    proc->acc = getchar();
    proc->pc++;
}

void asc()
{
	fprintf(stderr, "%#x asc\n",proc->pc);
    printf("%c",proc->acc);
    proc->pc++;
}

void clb()
{
    proc->acc = 0;
    proc->carry = 0;
	fprintf(stderr, "clb\n");
    proc->pc++;
}

void clc() //TODO: mit asc getauscht, weil da ganz offensichtlich n fehler ist...
{
/*    proc->carry = 0;
    proc->pc++;
    fprintf(stderr, "clc\n");*/
    asc();
}

void iac()
{
	fprintf(stderr, "iac\n");
	UBYTE tmp = proc->acc;
    if(tmp < ++proc->acc)
        proc->carry = true;
    proc->pc++;
}

void cmc()
{
	fprintf(stderr, "cmc\n");
    proc->carry = (proc->carry==false)?true:false;
    proc->pc++;
}

void cma()
{
	fprintf(stderr, "cma\n");
    proc->acc = ~proc->acc;
    proc->pc++;
}

void ral()
{
	fprintf(stderr, "ral\n");
	UBYTE lsb = proc->carry;
	proc->carry = proc->acc & 0x80; // carry = msb
	proc->acc = (proc->acc << 1) & lsb;
	
    proc->pc++;
}

void rar()
{
	fprintf(stderr, "rar\n");
	UBYTE msb = proc->carry;
	proc->carry = proc->acc & 0x01; // carry = lsb
	proc->acc = (proc->acc >> 1) & (msb << 7);
	
    proc->pc++;
}

void tcc()
{
	fprintf(stderr, "tcc\n");
    proc->acc = proc->carry;
    proc->pc++;
}

void dac()
{
	UBYTE tmp = proc->acc;
	fprintf(stderr, "dac\n");
    if(tmp > --proc->acc)
        proc->carry = true;
    proc->pc++;
}

void stc()
{
    proc->carry = true;
    fprintf(stderr, "stc\n");
    proc->pc++;
}

void daa() //TODO BCD adjust
{
	fprintf(stderr, "daa\n");
    proc->pc++;
}

void hlt()
{
    proc->run = 0;
    fprintf(stderr, "hlt\n");
}

