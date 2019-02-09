#include <iostream>

#include <lua.h>
#include <lauxlib.h>

#include "LuaScriptRead.hpp"
#include <restruct/RealizedNode.hpp>
#include <restruct/StructNodeRegistry.hpp>

void LuaScriptRead::operator()(RealizedNode* node) {
  std::cout << "CALL! Stack top: " << lua_gettop(this->registry->L) << std::endl;

  if(node->luaResultID)
    lua_pushinteger(this->registry->L, node->luaResultID); /* push ID for later */

  this->pcall(node, 1);

  if(node->luaResultID) {
    //lua_pushinteger called earlier for table key
    lua_settable(this->registry->L, LUA_REGISTRYINDEX); // ToS = chunk to run
  } else {
    node->luaResultID = luaL_ref(this->registry->L, LUA_REGISTRYINDEX);
  }
}
