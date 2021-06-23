#include "../src/lauxlib.h"

static int test1_state(struct lua_State* L)
{
    lua_Integer i = luaL_tointeger(L, -1);
    printf("test1 integer = %d\n", i);
    return 0;
}

void test1()
{
    struct lua_State* L = luaL_newstate();
    luaL_pushcfunction(L, &test1_state);
    luaL_pushinteger(L, 999);
    luaL_pushinteger(L, 888);
    luaL_pcall(L, 2, 0);
    luaL_close(L);
}

int main()
{
    test1();
    return 0;
}