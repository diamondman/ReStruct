#include <iostream>

#include <lua.h>
#include <lauxlib.h>

#include "LuaScript.hpp"
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

void LuaScript::pcall(RealizedNode* node, int nresults) {
  lua_pushinteger(this->registry->L, this->scriptID);  /* push address */
  lua_gettable(this->registry->L, LUA_REGISTRYINDEX); // ToS = chunk to run
  this->pushRealizedNode(node);
  if (lua_pcall(this->registry->L, 1, nresults, 0)) {
      fprintf(stderr, "Failed to run script: %s\n",
              lua_tostring(this->registry->L, -1));
      exit(1);
  }
}
