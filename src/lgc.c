#include "lgc.h"
#include "lmem.h"

#define gettotalbytes(g) (g->totalbytes + g->GCdebt)
#define white2gray(o) resetbits((o)->marked, WHITEBITS)
#define gray2black(o) l_setbit((o)->marked, BLACKBIT)
#define black2gray(o) resetbit((o)->marked, BLACKBIT

#define sweepwholelist(L, list) sweeplist(L, list, MAX_LUMEM)

struct GCObject* luaC_newobj(struct lua_State* L, int tt_, size_t size)
{
    struct global_State* g = G(L);
    struct GCObject* obj = (struct GCObject*)luaM_realloc(L, NULL, 0, size);
    obj->marked = luaC_white(g);
    obj->next = g->allgc;
    obj->tt_ = tt_;
    g->allgc = obj;

    return obj;
}

void reallymarkobject(struct lua_State* L, struct GCObject* gco)
{
    struct global_State* g = G(L);
    white2gray(gco);

    switch (gco->tt_)
    {
    case LUA_TTHREAD: 
        linkgclist(gco2th(gco), g->gray);
        break;
    case LUA_TSTRING:
        gray2black(gco);
        g->GCmemtrav += sizeof(struct TString);
        break;
    default:
        break;
    }
}

static void restart_collection(struct lua_State* L)
{
    struct global_State* g = G(L);
    g->gray = g->grayagain = NULL;
    markobject(L, g->mainthread);
}

static lu_mem traversethread(struct lua_State* L, struct lua_State* th)
{
    return 0;
}

static void propagatemark(struct lua_State* L)
{
    struct global_State* g = G(L);
    if (!g->gray) {
        return;
    }
    struct GCObject* gco = g->gray;
    gray2black(gco);
    lu_mem size = 0;

    switch (gco->tt_) {
    case LUA_TTHREAD: {
            black2gray(gco);
        }
        break;
    }
}

static void propagateall(struct lua_State* L)
{

}

static void atomic(struct lua_State* L)
{

}

static lu_mem freeobj(struct lua_State* L, struct GCObject* gco)
{
    return 0;
}

static struct GCObject** sweeplist(struct lua_State* L, struct GCObject** p, size_t count)
{
    struct global_State* g = G(L);
    lu_byte ow = otherwhite(g);
    while (*p != NULL && count > 0)
    {
        lu_byte marked = (*p)->marked;
        if (isdeadm(ow, marked))
        {
            struct GCObject* gco = *p;
            *p = (*p)->next;
            g->GCmemtrav += freeobj(L, gco);
        }
        else
        {
            (*p)->marked &= cast(lu_byte, ~(bitmask(BLACKBIT) | WHITEBITS));
            (*p)->marked |= luaC_white(g);
            p = &((*p)->next);
        }
        count--;
    }
    return (*p) == NULL ? NULL : p;
}

static void entersweep(struct lua_State* L) 
{

}

static void sweepstep(struct lua_State* L) 
{

}

static void setdebt(struct lua_State* L, l_mem debt)
{
    struct global_State* g = G(L);
    lu_mem totalbytes = gettotalbytes(g);

    g->totalbytes = totalbytes - debt;
    g->GCdebt = debt;
}

static void setpause(struct lua_State* L)
{
    struct global_State* g = G(L);
    l_mem estimate = g->GCestimate / GCPAUSE;
    estimate = (estimate * g->GCstepmul) >= MAX_LMEM ? MAX_LMEM : estimate * g->GCstepmul;

    l_mem debt = g->GCestimate - estimate;
    setdebt(L, debt);
}

static l_mem get_debt(struct lua_State* L) 
{
    struct global_State* g = G(L);

    return 0;
}

static lu_mem singlestep(struct lua_State* L)
{
    struct global_State* g = G(L);
    switch (g->gcstate)
    {
    case GCSpause: {
            g->GCmemtrav = 0;
            restart_collection(L);
            g->gcstate = GCSpropagate;
            return g->GCmemtrav;
        }
        break;
    case GCSpropagate: {
            g->GCmemtrav = 0;
            propagatemark(L);
            if (g->gray == NULL) {
                g->gcstate = GCSatomic;
            }
            return g->GCmemtrav;
        }
        break;
    case GCSatomic: {

        }
        break;
    case GCSsweepallgc: {

        }
        break;
    case GCSsweepend: {

        }
        break;
    default:
        break;
    }
}

void luaC_step(struct lua_State* L)
{
    struct global_State* g = G(L);
    l_mem debt = get_debt(L);
    do {
        l_mem work = singlestep(L);
        debt -= work;

        // 一次处理一个GCSTEPSIZE单位的数据
    } while(debt > -GCSTEPSIZE && g->gcstate != GCSpause);

    if (g->gcstate == GCSpause)
    {
        // GC清理完比
        setpause(L);
    }
    else
    {
        // GC只清理一部分
        debt = g->GCdebt / STEPMULADJ * g->GCstepmul;
        setdebt(L, debt);
    }
}

void luaC_freeallobjects(struct lua_State* L)
{
    struct global_State* g = G(L);
    g->currentwhite = WHITEBITS;
    sweepwholelist(L, &g->allgc);
}