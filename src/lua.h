#ifndef _LUA_H_
#define _LUA_H_

#include <stdarg.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

#include "luaconf.h"

#if defined(LLONG_MAX)
#define LUA_INTEGER long
#define LUA_NUMBER double
#else
#define LUA_INTEGER int 
#define LUA_NUMBER float 
#endif 

// ERROR CODE 
#define LUA_OK 0
#define LUA_ERRERR 1
#define LUA_ERRMEM 2
#define LUA_ERRRUN 3 

#define cast(t, exp) ((t)(exp))
#define savestack(L, o) ((o) - (L)->stack)
#define restorestack(L, o) ((L)->stack + (o)) 

// basic object type
#define LUA_TNUMBER 1
#define LUA_TLIGHTUSERDATA 2
#define LUA_TBOOLEAN 3
#define LUA_TSTRING 4
#define LUA_TNIL 5
#define LUA_TTABLE 6
#define LUA_TFUNCTION 7
#define LUA_TTHREAD 8
#define LUA_TNONE 9

// stack define
#define LUA_MINSTACK 20
#define LUA_STACKSIZE (2 * LUA_MINSTACK)
#define LUA_EXTRASTACK 5
#define LUA_MAXSTACK 15000
#define LUA_ERRORSTACK 200
#define LUA_MULRET -1
#define LUA_MAXCALLS 200

// error tips
#define LUA_ERROR(L, s) printf("LUA ERROR:%s", s);

typedef LUA_NUMBER lua_Number;
typedef LUA_INTEGER lua_Integer;

typedef struct lua_State lua_State;

typedef int (*lua_CFunction) (lua_State *L);
typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize);

struct lua_State* lua_newstate(lua_Alloc alloc, void* ud);
void lua_close(struct lua_State* L);

void increase_top(struct lua_State* L);
void lua_pushcfunction(struct lua_State* L, lua_CFunction f);
void lua_pushinteger(struct lua_State* L, int integer);
void lua_pushnumber(struct lua_State* L, float number);
void lua_pushboolean(struct lua_State* L, bool b);
void lua_pushnil(struct lua_State* L);
void lua_pushlightuserdata(struct lua_State* L, void* p);

lua_Integer lua_tointegerx(struct lua_State* L, int idx, int* isnum);
lua_Number lua_tonumberx(struct lua_State* L, int idx, int* isnum);
bool lua_toboolean(struct lua_State* L, int idx);
int lua_isnil(struct lua_State* L, int idx);

void lua_settop(struct lua_State* L, int idx);
int lua_gettop(struct lua_State* L);
void lua_pop(struct lua_State* L);

#endif // _LUA_H_
