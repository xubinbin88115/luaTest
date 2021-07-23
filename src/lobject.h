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

// lua Table
typedef union TKey {
    struct {
        Value value_;
        int tt_;
        int next;
    } nk;
    TValue tvk;
} TKey;

typedef struct Node {
    TKey key;
    TValue value;
} Node;

const Node dummynode_;

struct Table {
    CommonHeader;
    TValue* array;
    unsigned int arraysize;
    Node* node;
    unsigned int lsizenode; // real hash size is 2 ^ lsizenode
    Node* lastfree;
    struct GCObject* gclist;
};

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

#define twoto(lsize) (1 << (lsize))
#define lmod(hash, size) check_exp((size) & (size - 1) == 0, (hash) & (size - 1))

#define lua_numeq(a, b) ((a) == (b))
#define lua_numisnan(a) (!lua_numeq(a, a))
#define lua_numbertointeger(n, p) \
    (n >= cast(lua_Number, INT_MIN)) && \
    (n <= cast(lua_Number, INT_MAX)) && \
    ((*p = cast(lua_Integer, n)), 1)

#define ttisinteger(o) ((o)->tt_ == LUA_NUMINT)
#define ttisnumber(o) ((o)->tt_ == LUA_NUMFLT)
#define ttisshrstr(o) ((o)->tt_ == LUA_SHRSTR)
#define ttislngstr(o) ((o)->tt_ == LUA_LNGSTR)
#define ttisdeadkey(o) ((o)->tt_ == LUA_TDEADKEY)
#define ttistable(o) ((o)->tt_ == LUA_TTABLE)
#define ttisnil(o) ((o)->tt_ == LUA_TNIL)

void setivalue(StkId target, int integer);
void setfvalue(StkId target, lua_CFunction f);
void setfltvalue(StkId target, float number);
void setbvalue(StkId target, bool b);
void setnilvalue(StkId target);
void setpvalue(StkId target, void* p);
void setgco(StkId target, struct GCObject* gco);

void setobj(StkId target, StkId value);

int luaO_ceillog2(int value);

#endif // _LOBJECT_H_
