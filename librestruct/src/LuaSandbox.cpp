#include "LuaSandbox.hpp"
#include <stdlib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

static const luaL_Reg loadedlibs[] = {
  //{LUA_LOADLIBNAME, luaopen_package},
  //{LUA_COLIBNAME, luaopen_coroutine},
  {LUA_TABLIBNAME, luaopen_table},
  //{LUA_IOLIBNAME, luaopen_io},
  //{LUA_OSLIBNAME, luaopen_os},
  {LUA_STRLIBNAME, luaopen_string},
  {LUA_MATHLIBNAME, luaopen_math},
  {LUA_UTF8LIBNAME, luaopen_utf8},
  //{LUA_DBLIBNAME, luaopen_debug},
  {"_G", luaopen_base}, // HAS TO BE THE LAST ONE MADE CONSTANT
  {NULL, NULL}
};


static int SYS_newindex (lua_State *L) {
  return luaL_error(L, "Attempt to modify read-only table");
}

LUALIB_API void restruct_openlibs (lua_State *L) {
  // Load built in libraries
  for (const luaL_Reg *lib = loadedlibs; lib->func; lib++) {
    luaL_requiref(L, lib->name, lib->func, 1);
    lua_pop(L, 1);  /* remove lib */
  }

  // Remove unsafe operations from the global scope.
  #define LUA_INIT_SCRIPT R"~~~(
local newG = {utf8=utf8, string=string, math=math, xpcall=xpcall, error=error, type=type, _VERSION=_VERSION, pcall=pcall, next=next, _G=_G, assert=assert, pairs=pairs, tonumber=tonumber, ipairs=ipairs, select=select, tostring=tostring, table=table}
for k,v in pairs(newG) do
  print('Global key', k, '               value', v)
end
_G = newG
)~~~"
  auto res = luaL_dostring(L, LUA_INIT_SCRIPT);
  if(res) {
    fprintf(stderr, "Failed to run startup script: %s\n", lua_tostring(L, -1));
    exit(1);
  }

  // Making global tables immutable
  for (const luaL_Reg *lib = loadedlibs; lib->func; lib++) {
    //function readonlytable(table)
    //   return setmetatable({}, {
    //     __index = table,
    //     __newindex = function(table, key, value)
    //                    error("Attempt to modify read-only table")
    //                  end,
    //     __metatable = false
    //   });
    //end
    lua_createtable(L, 0, 0);
    lua_createtable(L, 3, 0);

    lua_pushstring(L, "__index");
    lua_getglobal(L, lib->name);
    lua_settable(L, -3);

    lua_pushstring(L, "__newindex");
    lua_pushcfunction(L, SYS_newindex);
    lua_settable(L, -3);

    lua_pushstring(L, "__metatable");
    lua_pushboolean(L, false);
    lua_settable(L, -3);

    lua_setmetatable(L, -2);
    lua_setglobal(L, lib->name);
  }
}
