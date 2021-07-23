#include "ltable.h"
#include "lgc.h"
#include "lmem.h"
#include "lstring.h"

static int l_hashfloat(lua_Number n) {
    int i = 0;
    lua_Integer ni = 0;

    n = frexp(n, &i) * -cast(lua_Number, INT_MIN);
    if (!lua_numbertointeger(n, &ni)) {
        lua_assert(lua_numisnan(n) || fabs(n) == cast(lua_Number, HUGE_VAL));
        return 0;
    }
    else {
        unsigned int u = cast(unsigned int, ni) + cast(unsigned int, i);
        return u <= INT_MAX ? u : ~u;
    }
}

static void setnodesize(struct lua_State* L, struct Table* t, int size) {
    if (size == 0) {
        t->lsizenode = 0;
        t->node = cast(Node*, dummynode);
        t->lastfree = NULL;
    }
    else {
        int lsize = luaO_ceillog2(size);
        t->lsizenode = (unsigned int)lsize;
        if (t->lsizenode > (sizeof(int) * CHAR_BIT - 1)) {
            luaD_throw(L, LUA_ERRRUN);
        }

        int node_size = twoto(lsize);
        t->node = (Node*)luaM_newvector(L, node_size, Node);
        t->lastfree = &t->node[node_size]; // it is not a bug, at the beginning, lastfree point to the address which next to last node
        
        for (int i = 0; i < node_size; i++) {
            Node* n = &t->node[i];
            TKey* k = &n->key;
            k->nk.next = 0;
            setnilvalue(getwkey(n));
            setnilvalue(getval(n));
        }
    }
}

struct Table* luaH_new(struct lua_State* L)
{
	struct GCObject* o = luaC_newobj(L, LUA_TTABLE, sizeof(struct Table));
    struct Table* t = gco2tbl(o);
    t->array = NULL;
    t->arraysize = 0;
    t->node = NULL;
    t->lsizenode = 0;
    t->lastfree = NULL;
    t->gclist = NULL;
    
    setnodesize(L, t, 0);
    return t;
}

void luaH_free(struct lua_State* L, struct Table* t) 
{
    if (t->arraysize > 0) {
        luaM_free(L, t->array, t->arraysize * sizeof(TValue));
    }

    if (!isdummy(t)) {
        luaM_free(L, t->node, twoto(t->lsizenode) * sizeof(struct Node));
    }

    luaM_free(L, t, sizeof(struct Table));
}

const TValue* luaH_getint(struct lua_State* L, struct Table* t, int key) 
{
    // 1 <= key <= arraysize
    if (cast(unsigned int, key) - 1 < t->arraysize) {
        return cast(const TValue*, &t->array[key - 1]);        
    }
    else {
        struct Node* n = hashint(key, t);
        while(true) {
            const TKey* k = &n->key;
            if (ttisinteger(&k->tvk) && k->tvk.value_.i == key) {
                return cast(const TValue*, getval(n));
            }
            else {
                int next = k->nk.next;
                if (next == 0) {
                    break;
                }
                n += next; 
            }
        }
    }

    return luaO_nilobject;
}

static Node* mainposition(struct lua_State* L, struct Table* t, const TValue* key) {
    switch(key->tt_) {
        case LUA_NUMINT: return hashint(key->value_.i, t); 
        case LUA_NUMFLT: return hashint(l_hashfloat(key->value_.n), t);
        case LUA_TBOOLEAN: return hashboolean(key->value_.b, t);
        case LUA_SHRSTR: return hashstr(gco2ts(gcvalue(key)), t);
        case LUA_LNGSTR: {
            struct TString* ts = gco2ts(gcvalue(key));
            luaS_hashlongstr(L, ts);
            return hashstr(ts, t);
        };
        case LUA_TLIGHTUSERDATA: {
            return hashpointer(key->value_.p, t);
        };
        case LUA_TLCF: {
            return hashpointer(key->value_.f, t);
        };
        default: {
            lua_assert(!ttisdeadkey(gcvalue(key))); 
            return hashpointer(gcvalue(key), t);
        };  
    } 
}

TValue* luaH_newkey(struct lua_State* L, struct Table* t, const TValue* key) {
    if (ttisnil(key)) {
        luaD_throw(L, LUA_ERRRUN);
    }

    TValue k;
    if (ttisnumber(key)) {
        if (lua_numisnan(key->value_.f)) {
            luaD_throw(L, LUA_ERRRUN);
        }

        k.value_.i = l_hashfloat(key->value_.n);
        k.tt_ = LUA_NUMINT;
        key = &k;
    }

    Node* main_node = mainposition(L, t, key);
    if (!ttisnil(getval(main_node)) || isdummy(t)) {
        Node* lastfree = getlastfree(t); 
        if (lastfree == NULL) {
            rehash(L, t, key); 
            return luaH_set(L, t, key);
        }

        Node* other_node = mainposition(L, t, getkey(main_node));
        if (other_node != main_node) {
            // find previous node of main node
            while(other_node + other_node->key.nk.next != main_node)
                other_node += other_node->key.nk.next;
            
            other_node->key.nk.next = lastfree - other_node;
            setobj(getwkey(lastfree), getwkey(main_node));
            setobj(getval(lastfree), getval(main_node));

            main_node->key.nk.next = 0;
            setnilvalue(getval(main_node));
        }
        else {
			if (main_node->key.nk.next != 0) {
				Node* next = main_node + main_node->key.nk.next;
				lastfree->key.nk.next = next - lastfree;
			}
            main_node->key.nk.next = lastfree - main_node;
            main_node = lastfree;
        }
    }
    
    setobj(getwkey(main_node), cast(TValue*, key));
    luaC_barrierback(L, t, key);
    lua_assert(ttisnil(getval(main_node)));
    return getval(main_node);
}

int luaH_setint(struct lua_State* L, struct Table* t, int key, const TValue* value) 
{
    const TValue* p = luaH_getint(L, t, key);
    TValue* cell = NULL;
    if (p != luaO_nilobject) {
        cell = cast(TValue*, p);
    }
    else {
        TValue k;
        setivalue(&k, key);
        cell = luaH_newkey(L, t, &k); 
    }
    
    cell->value_ = value->value_;
    cell->tt_ = value->tt_;
    return 1;
}