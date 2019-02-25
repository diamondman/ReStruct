#include <iostream>
#include <stdio.h>

#include <lua.h>

#include "LuaScriptToString.hpp"
#include <restruct/StructNodeRegistry.hpp>
#include <restruct/RealizedNode.hpp>

void LuaScriptToString::call_luares(RealizedNode* node) {
  std::cerr << "  Running Script " << node->getPath() << ".TOSTRING" << std::endl;
  this->printTOS();
  this->pcall(node, "ReStruct.nodeToStringContext", 1);
  this->printTOS();
  std::cerr << "  Script END " << node->getPath() << ".TOSTRING" << std::endl;
}

std::string LuaScriptToString::operator()(RealizedNode* node) {
  this->call_luares(node);

  std::string ret(lua_tostring(this->registry->L, 1));
  lua_pop(this->registry->L, 1);  /* Take the returned value out of the stack */
  return ret;
}
