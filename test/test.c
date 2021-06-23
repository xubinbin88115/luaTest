#include "../src/lauxlib.h"

static int test1_state(struct lua_State* L)
{
    lua_Integer i = luaL_tointeger(L, -1);
    printf("test1 integer = %d\n", i);
    return 0;
}

// 没有返回值
void test1()
{
    struct lua_State* L = luaL_newstate();
    luaL_pushcfunction(L, &test1_state);
    luaL_pushinteger(L, 999);
    luaL_pushinteger(L, 888);
    luaL_pcall(L, 2, 0);
    luaL_close(L);
}

static int test2_state(struct lua_State* L)
{
    lua_Integer a = luaL_tointeger(L, -1);
    lua_Integer b = luaL_tointeger(L, -2);
    lua_Integer sum = a + b;
    luaL_pushinteger(L, sum);
    return 1;
}

// 一个返回值
void test2()
{
    struct lua_State* L = luaL_newstate();
    printf("test2 stacksize = %d\n", luaL_stacksize(L));
    luaL_pushcfunction(L, &test2_state);
    luaL_pushinteger(L, 999);
    luaL_pushinteger(L, 888);
    printf("test2 stacksize = %d\n", luaL_stacksize(L));
    luaL_pcall(L, 2, 1);
    printf("test2 stacksize = %d\n", luaL_stacksize(L));

    lua_Integer sum = luaL_tointeger(L, -1);
    printf("test2 integer sum = %d\n", sum);
    // luaL_pop(L);
    lua_settop(L, -1);
    printf("test2 stacksize = %d\n", luaL_stacksize(L));

    luaL_close(L);
}

static int test3_state(struct lua_State* L)
{
    luaL_pushinteger(L, 111);
    luaL_pushinteger(L, 222);
    luaL_pushinteger(L, 333);
    return 3;
}

// 指定返回1个，但实际3个返回值
void test3()
{
    struct lua_State* L = luaL_newstate();
    printf("test3 stacksize = %d\n", luaL_stacksize(L));
    luaL_pushcfunction(L, &test3_state);
    luaL_pushinteger(L, 999);
    printf("test3 stacksize = %d\n", luaL_stacksize(L));
    luaL_pcall(L, 1, 1);
    printf("test3 stacksize = %d\n", luaL_stacksize(L));

    lua_Integer a = luaL_tointeger(L, -3);
    printf("test3 integer ret1 = %d\n", a);

    lua_Integer b = luaL_tointeger(L, -2);
    printf("test3 integer ret2 = %d\n", b);
    
    lua_Integer c = luaL_tointeger(L, -1);
    printf("test3 integer ret3 = %d\n", c);

    lua_settop(L, -1);
    printf("test3 stacksize = %d\n", luaL_stacksize(L));

    luaL_close(L);
}

static int test4_state(struct lua_State* L)
{
    luaL_pushinteger(L, 111);
    return 1;
}

// 指定返回3个，但实际1个返回值
void test4()
{
    struct lua_State* L = luaL_newstate();
    printf("test4 stacksize = %d\n", luaL_stacksize(L));
    luaL_pushcfunction(L, &test4_state);
    luaL_pushinteger(L, 999);
    printf("test4 stacksize = %d\n", luaL_stacksize(L));
    luaL_pcall(L, 1, 3);
    printf("test4 stacksize = %d\n", luaL_stacksize(L));

    lua_Integer a = luaL_tointeger(L, -3);
    printf("test4 integer ret1 = %d\n", a);

    lua_Integer b = luaL_tointeger(L, -2);
    printf("test4 integer ret2 = %d\n", b);
    
    lua_Integer c = luaL_tointeger(L, -1);
    printf("test4 integer ret3 = %d\n", c);

    lua_settop(L, -3);
    printf("test4 stacksize = %d\n", luaL_stacksize(L));

    luaL_close(L);
}

int main()
{
    // test1();
    // test2();
    // test3();
    test4();
    return 0;
}