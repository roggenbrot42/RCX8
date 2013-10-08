#ifndef TYPES_H
#define TYPES_H

#define _REGISTERCOUNT_ 8
#define _MEMSIZE_ 65536
#define _CALLSTACKSIZE_ 128

typedef unsigned short ADDR;
typedef unsigned short WORD;
typedef char BYTE;
typedef unsigned char REGNO;
typedef unsigned char UBYTE;
typedef struct RCX8{
	UBYTE reg[_REGISTERCOUNT_];
	UBYTE memory[_MEMSIZE_];
	ADDR stack[_CALLSTACKSIZE_];
	UBYTE acc;
	ADDR pc;
	UBYTE spointer;
	UBYTE carry;
	UBYTE run;
} * PRCX8, RCX8;

#endif
