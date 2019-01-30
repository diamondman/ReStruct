#include <iostream>

#include "restruct/restruct.hpp"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <stdio.h>

//int luamain(void)
//{
//    int status, result, i;
//    double sum;
//    lua_State *L;
//
//    /*
//     * All Lua contexts are held in this structure. We work with it almost
//     * all the time.
//     */
//    L = luaL_newstate();
//
//    luaL_openlibs(L); /* Load Lua libraries */
//
//    /* Load the file containing the script we are going to run */
//    status = luaL_loadfile(L, "script.lua");
//    if (status) {
//        /* If something went wrong, error message is at the top of */
//        /* the stack */
//        fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
//        exit(1);
//    }
//
//    /*
//     * Ok, now here we go: We pass data to the lua script on the stack.
//     * That is, we first have to prepare Lua's virtual stack the way we
//     * want the script to receive it, then ask Lua to run it.
//     */
//    lua_newtable(L);    /* We will pass a table */
//
//    /*
//     * To put values into the table, we first push the index, then the
//     * value, and then call lua_rawset() with the index of the table in the
//     * stack. Let's see why it's -3: In Lua, the value -1 always refers to
//     * the top of the stack. When you create the table with lua_newtable(),
//     * the table gets pushed into the top of the stack. When you push the
//     * index and then the cell value, the stack looks like:
//     *
//     * <- [stack bottom] -- table, index, value [top]
//     *
//     * So the -1 will refer to the cell value, thus -3 is used to refer to
//     * the table itself. Note that lua_rawset() pops the two last elements
//     * of the stack, so that after it has been called, the table is at the
//     * top of the stack.
//     */
//    for (i = 1; i <= 5; i++) {
//        lua_pushnumber(L, i);   /* Push the table index */
//        lua_pushnumber(L, i*2); /* Push the cell value */
//        lua_rawset(L, -3);      /* Stores the pair in the table */
//    }
//
//    /* By what name is the script going to reference our table? */
//    lua_setglobal(L, "foo");
//
//    /* Ask Lua to run our little script */
//    result = lua_pcall(L, 0, LUA_MULTRET, 0);
//    if (result) {
//        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
//        exit(1);
//    }
//
//    /* Get the returned value at the top of the stack (index -1) */
//    sum = lua_tonumber(L, -1);
//
//    printf("Script returned: %.0f\n", sum);
//
//    lua_pop(L, 1);  /* Take the returned value out of the stack */
//    lua_close(L);   /* Cya, Lua */
//
//    return 0;
//}
#include <stdio.h>
#include <stdint.h>
int luamain(void)
{
    int status, result, i;
    double sum;
    lua_State *L;

    L = luaL_newstate();

    luaL_openlibs(L);

    static const char S1key = '1';
    lua_pushlightuserdata(L, (void*)&S1key);
    status = luaL_loadfile(L, "script.lua");
    if (status) {
        fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
        exit(1);
    }
    lua_settable(L, LUA_REGISTRYINDEX);


    static const char S2key = '2';
    lua_pushlightuserdata(L, (void*)&S2key);
    status = luaL_loadfile(L, "script2.lua");
    if (status) {
        fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
        exit(1);
    }
    lua_settable(L, LUA_REGISTRYINDEX);


    lua_pushlightuserdata(L, (void *)&S1key);  /* push address */
    lua_gettable(L, LUA_REGISTRYINDEX);

    uint8_t x = -2;
    printf("C: 0x%02x\n", x);
    lua_pushinteger(L, x);
    lua_setglobal(L, "x");

    const unsigned char y[] = {0x10, 0x40, 0x51, 0x00, 0x12, 0xff, 0x11};
    //printf("C: 0x%02x\n", a);
    lua_pushlstring(L, (const char *)y, sizeof(y));
    lua_setglobal(L, "y");

    if (lua_pcall(L, 0, LUA_MULTRET, 0)) {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        exit(1);
    }
    int8_t ret = lua_tointeger(L, -1);
    printf("Script returned: 0x%x, %d\n", ret, ret);
    lua_pop(L, 1);  /* Take the returned value out of the stack */



    //lua_pushlightuserdata(L, (void *)&S2key);  /* push address */
    //lua_gettable(L, LUA_REGISTRYINDEX);
    //if (lua_pcall(L, 0, LUA_MULTRET, 0)) {
    //    fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
    //    exit(1);
    //}
    //sum = lua_tonumber(L, -1);
    //printf("Script returned: %.0f\n", sum);
    //lua_pop(L, 1);  /* Take the returned value out of the stack */
    //
    //
    //
    //lua_pushlightuserdata(L, (void *)&S1key);  /* push address */
    //lua_gettable(L, LUA_REGISTRYINDEX);
    //if (lua_pcall(L, 0, LUA_MULTRET, 0)) {
    //    fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
    //    exit(1);
    //}
    //sum = lua_tonumber(L, -1);
    //printf("Script returned: %.0f\n", sum);
    //lua_pop(L, 1);  /* Take the returned value out of the stack */



    lua_close(L);

    return 0;
}

int restruct_main(int argc, char** argv) {
  luamain();
  return 0;
}
