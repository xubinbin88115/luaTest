#ifndef _LMEM_H_
#define _LMEM_H_

#include "lstate.h"

#define luaM_free(L, ptr, osize) luaM_realloc(L, ptr, osize, 0)
#define luaM_reallocvector(L, ptr, o, n, t) ((ptr) = (t*)luaM_realloc(L, ptr, o * sizeof(t), n * sizeof(t)))

void* luaM_realloc(struct lua_State* L, void* ptr, size_t osize, size_t nsize);

#endif // _LMEM_H_
