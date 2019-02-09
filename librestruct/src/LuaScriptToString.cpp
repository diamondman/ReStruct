#include <iostream>
#include <stdio.h>

#include <lua.h>

#include "LuaScriptToString.hpp"
#include <restruct/StructNodeRegistry.hpp>

void LuaScriptToString::call_luares(RealizedNode* node) {
  this->pcall(node, 1);
}

std::string LuaScriptToString::operator()(RealizedNode* node) {
  this->call_luares(node);

  std::string ret(lua_tostring(this->registry->L, 1));
  lua_pop(this->registry->L, 1);  /* Take the returned value out of the stack */
  return ret;
}
