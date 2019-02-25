#include <iostream>

#include <lua.h>
#include <lauxlib.h>

#include "LuaScriptRead.hpp"
#include <restruct/RealizedNode.hpp>
#include <restruct/StructNodeRegistry.hpp>

void LuaScriptRead::operator()(RealizedNode* node) {
  std::cerr << "  Running Script " << node->getPath() << ".READ" << std::endl;
  this->printTOS();

  if(node->luaResultID)
    lua_pushinteger(this->registry->L, node->luaResultID); /* push ID for later */

  this->pcall(node, "ReStruct.leafNodeReadStreamContext", 1);

  if(node->luaResultID) {
    //lua_pushinteger called earlier for table key
    lua_settable(this->registry->L, LUA_REGISTRYINDEX); // ToS = chunk to run
  } else {
    node->luaResultID = luaL_ref(this->registry->L, LUA_REGISTRYINDEX);
  }

  this->printTOS();
  std::cerr << "  Script END " << node->getPath() << ".READ" << std::endl;
}
