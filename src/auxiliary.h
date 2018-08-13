#ifndef AUX_H
#define AUX_h

#include <inttypes.h>

inline uint16_t swapbytes(uint16_t i)
{
    return (i<<8)|(i>>8);
}

#endif
