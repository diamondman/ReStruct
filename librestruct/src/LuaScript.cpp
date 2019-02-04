#include <iostream>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <restruct/LuaScript.hpp>
#include <restruct/StructNodeRegistry.hpp>

LuaScript::LuaScript(StructNodeRegistry* registry_, std::string src)
  : registry(registry_) {
  this->scriptID = this->registry->registerScript(src);
  std::cout << "******LUA SCRIPT ALLOCATING " << this->scriptID << std::endl;
}

LuaScript::~LuaScript() {
  std::cout << "******LUA SCRIPT DEALLOCATING " << this->scriptID << std::endl;
  this->registry->unRegisterScript(this->scriptID);
}


void LuaScript::pushIntParam(const char* name, int param) {
  lua_pushinteger(this->registry->L, param);
  lua_setglobal(this->registry->L, name);
}

void LuaScript::pushNumberParam(const char* name, double param) {
  lua_pushnumber(this->registry->L, param);
  lua_setglobal(this->registry->L, name);
}

void LuaScript::pushRealizedNode(const char* name, RealizedNode* node) {
  lua_pushlightuserdata(this->registry->L, node);
  lua_setglobal(this->registry->L, name);
}

void LuaScript::pushStringParam(const char* name, const void* str, int strlen) {
  if(strlen == -1)
    lua_pushstring(this->registry->L, (const char *)str);
  else
    lua_pushlstring(this->registry->L, (const char *)str, strlen);

  lua_setglobal(this->registry->L, name);
}

void LuaScript::call(int* resultID) {
  std::cout << "CALL! Stack top: " << lua_gettop(this->registry->L) << std::endl;
  if(!resultID) return;

  if(*resultID)
    lua_pushinteger(this->registry->L, *resultID); /* push resultID for later */

  lua_pushinteger(this->registry->L, this->scriptID);  /* push address */
  lua_gettable(this->registry->L, LUA_REGISTRYINDEX); // ToS = chunk to run
  if (lua_pcall(this->registry->L, 0, 1, 0)) {
      fprintf(stderr, "Failed to run script: %s\n",
              lua_tostring(this->registry->L, -1));
      exit(1);
  }

  if(*resultID) {
    //lua_pushinteger called earlier for table key
    lua_settable(this->registry->L, LUA_REGISTRYINDEX); // ToS = chunk to run
  } else {
    *resultID = luaL_ref(this->registry->L, LUA_REGISTRYINDEX);
  }
  //std::cout << "stored res in " << *resultID << std::endl;
  //lua_pop(this->registry->L, 1);  /* Take the returned value out of the stack */
}

#include <iostream>
#include <stdio.h>

void LuaScript::calls_luares(RealizedNode* node, int luaResultID) {
  this->pushRealizedNode("rs", node);

  //std::cout << "luaResultID: " << luaResultID << std::endl;
  //lua_pushinteger(this->registry->L, luaResultID);

  lua_pushinteger(this->registry->L, luaResultID);
  lua_gettable(this->registry->L, LUA_REGISTRYINDEX); // ToS = chunk to run
  lua_setglobal(this->registry->L, "nodeval");

  lua_pushinteger(this->registry->L, this->scriptID);  /* push address */
  lua_gettable(this->registry->L, LUA_REGISTRYINDEX); // ToS = chunk to run
  if (lua_pcall(this->registry->L, 0, 1, 0)) {
      fprintf(stderr, "Failed to run script: %s\n",
              lua_tostring(this->registry->L, -1));
      exit(1);
  }
}

std::string LuaScript::calls(RealizedNode* node, int luaResultID) {
  this->calls_luares(node, luaResultID);

  std::string ret(lua_tostring(this->registry->L, 1));
  lua_pop(this->registry->L, 1);  /* Take the returned value out of the stack */
  return ret;
}

void LuaScript::callz(RealizedNode* node) {
  this->pushRealizedNode("rs", node);

  lua_pushinteger(this->registry->L, this->scriptID);  /* push address */
  lua_gettable(this->registry->L, LUA_REGISTRYINDEX); // ToS = chunk to run
  if (lua_pcall(this->registry->L, 0, 0, 0)) {
      fprintf(stderr, "Failed to run script: %s\n",
              lua_tostring(this->registry->L, -1));
      exit(1);
  }
}
