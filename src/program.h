#ifndef RCX8_H
#define RCX8_h

#include<inttypes.h>

#define _REGISTERCOUNT_ 8
#define _MEMSIZE_ 65536
#define _CALLSTACKSIZE_ 128

struct RCX8{
    uint8_t reg[_REGISTERCOUNT_];
    uint8_t memory[_MEMSIZE_];
    uint16_t stack[_CALLSTACKSIZE_];
    uint8_t acc;
    uint16_t pc;
    uint8_t spointer;
    uint8_t carry;
    uint8_t run;

};

#ifdef __cplusplus
extern "C" {
#endif

    void reset(struct RCX8*);
    void read_op(struct RCX8*);
    uint8_t get_regno(uint8_t);
    uint16_t fetch_addr(struct RCX8*,uint16_t);

    void fimzer(struct RCX8*,uint8_t);
    void incino(struct RCX8*,uint8_t);
    void iszsto(struct RCX8*,uint8_t);
    void addxor(struct RCX8*,uint8_t);
    void substi(struct RCX8*,uint8_t);
    void wmprdr(struct RCX8*,uint8_t);

    void nop(struct RCX8*);
    void jcn(struct RCX8*,uint8_t);
    void fin(struct RCX8*,uint8_t);
    void fim(struct RCX8*,uint8_t);
    void zer(struct RCX8*,uint8_t);
    void ftc(struct RCX8*,uint8_t);
    void jun(struct RCX8*,uint8_t);
    void jms(struct RCX8*,uint8_t);
    void inc(struct RCX8*,uint8_t);
    void ino(struct RCX8*,uint8_t);
    void isz(struct RCX8*,uint8_t);
    void sto(struct RCX8*,uint8_t);
    void add(struct RCX8*,uint8_t);
    void x0r(struct RCX8*,uint8_t);
    void sub(struct RCX8*,uint8_t);
    void sti(struct RCX8*,uint8_t);
    void ld(struct RCX8*,uint8_t);
    void xch(struct RCX8*,uint8_t);
    void bbl(struct RCX8*,uint8_t);
    void ldm(struct RCX8*,uint8_t);
    void wmp(struct RCX8*);
    void rdr(struct RCX8*);
    void kbi(struct RCX8*);
    void asc(struct RCX8*);
    void clb(struct RCX8*);
    void clc(struct RCX8*);
    void iac(struct RCX8*);
    void cmc(struct RCX8*);
    void cma(struct RCX8*);
    void ral(struct RCX8*);
    void rar(struct RCX8*);
    void tcc(struct RCX8*);
    void dac(struct RCX8*);
    void stc(struct RCX8*);
    void daa(struct RCX8*);
    void hlt(struct RCX8*);

#ifdef __cplusplus
}
#endif

#endif
