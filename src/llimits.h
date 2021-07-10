#ifndef _LLIMITS_H_
#define _LLIMITS_H_

#include <limits.h>
#include <stddef.h>

typedef unsigned char lu_byte;

typedef size_t      lu_mem;
typedef ptrdiff_t   l_mem;

#define MAX_LUMEM ((lu_mem)(~(lu_mem)0))
#define MAX_LMEM  (MAX_LUMEM >> 1)

#define lua_assert(c) ((void)0)
#define check_exp(c, e) (lua_assert(c), e)

#endif // _LLIMITS_H_
