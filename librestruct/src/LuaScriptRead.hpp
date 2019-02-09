#pragma once

#include "LuaScript.hpp"

class LuaScriptRead : private LuaScript {
public:
  LuaScriptRead(StructNodeRegistry* registry_, std::string src) :
    LuaScript(registry_, src) {};
  void operator()(RealizedNode* node);
};
