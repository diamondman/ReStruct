#include <iostream>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <restruct/LuaScript.hpp>
#include <restruct/RealizedNode.hpp>
#include <restruct/StructNodeRegistry.hpp>

LuaScript::LuaScript(StructNodeRegistry* registry_, std::string src)
  : registry(registry_) {
  this->scriptID = this->registry->registerScript(src);
  std::cerr << "******LUA SCRIPT ALLOCATING " << this->scriptID << std::endl;
}

LuaScript::~LuaScript() {
  std::cerr << "******LUA SCRIPT DEALLOCATING " << this->scriptID << std::endl;
  this->registry->unRegisterScript(this->scriptID);
}

void LuaScript::pushRealizedNode(RealizedNode* node) {
  lua_pushlightuserdata(this->registry->L, node);
  luaL_getmetatable(this->registry->L, "ReStruct.node");
  lua_setmetatable(this->registry->L, -2);
}

void LuaScript::call(RealizedNode* node) {
  std::cout << "CALL! Stack top: " << lua_gettop(this->registry->L) << std::endl;

  if(node->luaResultID)
    lua_pushinteger(this->registry->L, node->luaResultID); /* push ID for later */

  lua_pushinteger(this->registry->L, this->scriptID);  /* push address */
  lua_gettable(this->registry->L, LUA_REGISTRYINDEX); // ToS = chunk to run
  this->pushRealizedNode(node);
  if (lua_pcall(this->registry->L, 1, 1, 0)) {
      fprintf(stderr, "Failed to run script: %s\n",
              lua_tostring(this->registry->L, -1));
      exit(1);
  }

  if(node->luaResultID) {
    //lua_pushinteger called earlier for table key
    lua_settable(this->registry->L, LUA_REGISTRYINDEX); // ToS = chunk to run
  } else {
    node->luaResultID = luaL_ref(this->registry->L, LUA_REGISTRYINDEX);
  }
}

#include <iostream>
#include <stdio.h>

void LuaScript::calls_luares(RealizedNode* node) {
  lua_pushinteger(this->registry->L, this->scriptID);  /* push address */
  lua_gettable(this->registry->L, LUA_REGISTRYINDEX); // ToS = chunk to run
  this->pushRealizedNode(node);
  if (lua_pcall(this->registry->L, 1, 1, 0)) {
      fprintf(stderr, "Failed to run script: %s\n",
              lua_tostring(this->registry->L, -1));
      exit(1);
  }
}

std::string LuaScript::calls(RealizedNode* node) {
  this->calls_luares(node);

  std::string ret(lua_tostring(this->registry->L, 1));
  lua_pop(this->registry->L, 1);  /* Take the returned value out of the stack */
  return ret;
}

void LuaScript::callz(RealizedNode* node) {
  lua_pushinteger(this->registry->L, this->scriptID);  /* push address */
  lua_gettable(this->registry->L, LUA_REGISTRYINDEX); // ToS = chunk to run
  this->pushRealizedNode(node);
  if (lua_pcall(this->registry->L, 1, 0, 0)) {
      fprintf(stderr, "Failed to run script: %s\n",
              lua_tostring(this->registry->L, -1));
      exit(1);
  }
}
