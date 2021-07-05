#ifndef _LLIMITS_H_
#define _LLIMITS_H_

typedef unsigned char lu_byte;

typedef size_t      lu_mem;
typedef ptrdiff_t   l_mem;

#define MAX_LUMEM ((lu_mem)(~(lu_mem)0))
#define MAX_LMEM  (MAX_LUMEM >> 1)

#endif // _LLIMITS_H_