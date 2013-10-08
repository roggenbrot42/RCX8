#ifndef RCX8_H
#define RCX8_h

#include "types.h"

PRCX8 proc;

void (*oprmap[16])(UBYTE);
void (*opamap[16])();

void mapopc();
void reset();
void readOp();
REGNO getRegno(BYTE);
ADDR fetchADDR(ADDR);

void fimzer(UBYTE);
void incino(UBYTE);
void iszsto(UBYTE);
void addxor(UBYTE);
void substi(UBYTE);
void wmprdr(UBYTE);
void opa(UBYTE);

void nop(UBYTE);
void jcn(UBYTE);
void fin(REGNO);
void fim(REGNO);
void zer(REGNO);
void ftc(REGNO);
void jun(UBYTE);
void jms(UBYTE);
void inc(REGNO);
void ino(REGNO);
void isz(REGNO);
void sto(REGNO);
void add(REGNO);
void xor(REGNO);
void sub(REGNO);
void sti(REGNO);
void ld(REGNO);
void xch(REGNO);
void bbl(UBYTE);
void ldm(UBYTE);
void wmp();
void rdr();
void kbi();
void asc();
void clb();
void clc();
void iac();
void cmc();
void cma();
void ral();
void rar();
void tcc();
void dac();
void stc();
void daa();
void hlt();

#endif
