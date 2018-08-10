#ifndef LOADER_H
#define LOADER_H

struct RCX8* bootstrap(const char * name);
void setup(struct RCX8*);
void dumpstate(struct RCX8*);

#endif

