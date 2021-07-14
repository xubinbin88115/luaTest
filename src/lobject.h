#ifndef _LOBJECT_H_ 
#define _LOBJECT_H_

#include "llimits.h"
#include "lua.h"

// luaobject.h
#define CommonHeader struct GCObject* next; lu_byte tt_; lu_byte marked
#define LUA_GCSTEPMUL 200

#define MAXSHORTSTR 40

struct GCObject {
    CommonHeader;
};

typedef union lua_Value {
    struct GCObject* gc;
    void* p;
    int b;
    lua_Integer i;
    lua_Number n;
    lua_CFunction f;
} Value;

typedef struct lua_TValue {
    Value value_;
    int tt_;
} TValue;

typedef struct TString {
    CommonHeader;
    unsigned int hash;          // string hash value

    // if TString is long string type, then extra = 1 means it has been hash,
    // extra = 0 means it has not hash yet. if TString is short string type,
    // then extra = 0 means it can be reclaim by gc, or if extra is not 0,
    // gc can not reclaim it.
    unsigned short extra;
    unsigned short shrlen;
    union {
        struct TString* hnext; // only for short string, if two different string encounter hash collision, then chain them
        size_t lnglen;
    } u;
    char data[0];
} TString;

typedef TValue *StkId;

// lua number type 
#define LUA_NUMINT (LUA_TNUMBER | (0 << 4))
#define LUA_NUMFLT (LUA_TNUMBER | (1 << 4))

// lua function type 
#define LUA_TLCL (LUA_TFUNCTION | (0 << 4))
#define LUA_TLCF (LUA_TFUNCTION | (1 << 4))
#define LUA_TCCL (LUA_TFUNCTION | (2 << 4))

// string type 
#define LUA_LNGSTR (LUA_TSTRING | (0 << 4))
#define LUA_SHRSTR (LUA_TSTRING | (1 << 4))

void setivalue(StkId target, int integer);
void setfvalue(StkId target, lua_CFunction f);
void setfltvalue(StkId target, float number);
void setbvalue(StkId target, bool b);
void setnilvalue(StkId target);
void setpvalue(StkId target, void* p);

void setobj(StkId target, StkId value);

#endif // _LOBJECT_H_
