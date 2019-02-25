#include <iostream>

#include <lua.h>
#include <lauxlib.h>

#include "LuaScript.hpp"
#include <restruct/RealizedNode.hpp>
#include <restruct/StructNodeRegistry.hpp>

LuaScript::LuaScript(StructNodeRegistry* registry_, std::string src)
  : registry(registry_) {
  this->scriptID = this->registry->registerScript(src);
  //std::cerr << "******LUA SCRIPT ALLOCATING " << this->scriptID << std::endl;
}

LuaScript::~LuaScript() {
  //std::cerr << "******LUA SCRIPT DEALLOCATING " << this->scriptID << std::endl;
  this->registry->unRegisterScript(this->scriptID);
}

void LuaScript::pushRealizedNode(RealizedNode* node, const char* type) {
  LuaScriptNodeContext* datum = (LuaScriptNodeContext*)lua_newuserdata
    (this->registry->L, sizeof(LuaScriptNodeContext));
  datum->datum = node;
  luaL_getmetatable(this->registry->L, type);
  lua_setmetatable(this->registry->L, -2);
}

void LuaScript::pcall(RealizedNode* node, const char* type, int nresults) {
  lua_pushinteger(this->registry->L, this->scriptID);  /* push address */
  //printTOS();
  lua_gettable(this->registry->L, LUA_REGISTRYINDEX); // ToS = chunk to run
  //printTOS();
  this->pushRealizedNode(node, type);
  //printTOS();
  if (lua_pcall(this->registry->L, 1, nresults, 0)) {
      fprintf(stderr, "Node: %s; Failed to run script: %s\n",
              node->getPath().c_str(), lua_tostring(this->registry->L, -1));
      exit(1);
  }
}

void LuaScript::printTOS() {
  //std::cerr << "CALL! Stack top: " << lua_gettop(this->registry->L) << std::endl;
  //this->registry->stackDump();
}
