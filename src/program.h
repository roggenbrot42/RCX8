#ifndef RCX8_H
#define RCX8_H

#include<inttypes.h>

#define _REGISTERCOUNT_ 8
#define _MEMSIZE_ 65536
#define _CALLSTACKSIZE_ 128
#define _PORTNUM_ 256

struct RCX8{
    uint8_t reg[_REGISTERCOUNT_];
    uint8_t memory[_MEMSIZE_];
    uint16_t stack[_CALLSTACKSIZE_];
    uint8_t port[_PORTNUM_];
    uint8_t acc;
    uint16_t pc;
    uint8_t spointer;
    uint8_t carry;
    uint8_t test;
    uint8_t run;
    char * buf_opstr;
};

enum arg_types{
    arg_none,
    arg_reg,
    arg_dirct,
    arg_cnd,
    arg_addr,
    arg_ptr,
    arg_port,
    num_argst
};


struct OP{
    uint8_t opid;
    uint8_t opr;
    uint8_t opa;
    uint16_t arg[2];
    uint8_t arg_t[2];
};


enum opcode_mnemonic{
    _nop,
    _jcn,
    _fin,
    _fim,
    _zer,
    _ftc,
    _jun,
    _jms,
    _inc,
    _ino,
    _isz,
    _sto,
    _add,
    _x0r,
    _sub,
    _sti,
    _ld,
    _xch,
    _bbl,
    _ldm,
    _wmp,
    _rdr,
    _kbi,
    _asc,
    _clb,
    _clc,
    _iac,
    _cmc,
    _cma,
    _ral,
    _rar,
    _tcc,
    _dac,
    _stc,
    _daa,
    _hlt,
    _num_ops
};

extern const char * const op_name[];

#ifdef __cplusplus
extern "C" {
#endif

    void reset(struct RCX8*);
    struct OP read_op(struct RCX8 *);
    struct RCX8 execute_op(struct OP, struct RCX8);
    struct RCX8 * commit_op(struct RCX8);

    uint8_t get_regno(uint8_t);
    char * op2str(struct OP);
    uint16_t fetch_addr(struct RCX8*,uint16_t);
    extern char kbi_abstract();
    extern void asc_abstract(char);

    void fimzer(struct RCX8*,uint8_t);
    void incino(struct RCX8*,uint8_t);
    void iszsto(struct RCX8*,uint8_t);
    void addxor(struct RCX8*,uint8_t);
    void substi(struct RCX8*,uint8_t);
    void wmprdr(struct RCX8*,uint8_t);

    void nop(struct RCX8*);
    void jcn(struct RCX8*, uint8_t,uint16_t);
    void fin(struct RCX8*,uint8_t);
    void fim(struct RCX8*, uint8_t, uint8_t dirct);
    void zer(struct RCX8*,uint8_t);
    void ftc(struct RCX8*, uint8_t, uint16_t);
    void jun(struct RCX8*, uint16_t addr);
    void jms(struct RCX8*, uint16_t addr);
    void inc(struct RCX8*,uint8_t);
    void ino(struct RCX8*,uint8_t);
    void isz(struct RCX8*, uint8_t, uint16_t addr);
    void sto(struct RCX8*, uint8_t, uint16_t);
    void add(struct RCX8*,uint8_t);
    void x0r(struct RCX8*,uint8_t);
    void sub(struct RCX8*,uint8_t);
    void sti(struct RCX8*,uint8_t);
    void ld(struct RCX8*,uint8_t);
    void xch(struct RCX8*,uint8_t);
    void bbl(struct RCX8*, uint8_t );
    void ldm(struct RCX8*,uint8_t);
    void wmp(struct RCX8*, uint8_t port);
    void rdr(struct RCX8*, uint8_t port);
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
