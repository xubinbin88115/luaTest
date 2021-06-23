#ifndef _LSTATE_H_
#define _LSTATE_H_

#include "lobject.h"

#define LUA_EXTRASPACE sizeof(void*) 
#define G(L) ((L)->l_G)

typedef TValue* StkId;

struct CallInfo {
    StkId func;                     // 被调用函数在栈中的位置
    StkId top;                      // 被调用函数的栈顶位置
    int nresult;                    // 有多少个返回值
    int callstatus;                 // 调用状态
    struct CallInfo* next;          // 下一个调用
    struct CallInfo* previous;      // 上一个调用
};

typedef struct lua_State {
    StkId stack;
    StkId stack_last;
    StkId top;
    int stack_size;
    struct lua_longjmp* errorjmp;
    int status;
    struct lua_State* next;
    struct lua_State* previous;
    struct CallInfo base_ci;
    struct CallInfo* ci;
    struct global_State* l_G;
    ptrdiff_t errorfunc;
    int ncalls;
} lua_State;

typedef struct global_State {
    struct lua_State* mainthread;
    lua_Alloc frealloc;
    void* ud; 
    lua_CFunction panic;
} global_State;

struct lua_longjmp;

#endif // _LSTATE_H_