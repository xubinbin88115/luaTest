#ifndef _LAPI_H_
#define _LAPI_H_

#include "lstate.h"

TValue* luaL_index2addr(struct lua_State* L, int idx);

#endif // _LAPI_H_