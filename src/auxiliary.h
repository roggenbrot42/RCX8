#ifndef AUX_H
#define AUX_h

inline unsigned short swapbytes(unsigned short i)
{
    return (i<<8)|(i>>8);
}

#endif
