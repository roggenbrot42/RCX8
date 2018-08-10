#ifndef LOADER_H
#define LOADER_H

#ifdef __cplusplus
extern "C"{
#endif

struct RCX8* bootstrap(const char * name);
extern void setup(struct RCX8*);
extern void dumpstate(struct RCX8*);

#ifdef __cplusplus
}
#endif

#endif

