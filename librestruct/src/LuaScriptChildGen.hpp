#pragma once

#include "LuaScript.hpp"

class LuaScriptChildGen : private LuaScript {
public:
  LuaScriptChildGen(StructNodeRegistry* registry_, std::string src) :
    LuaScript(registry_, src) {};
  void operator()(RealizedNode* node);
};
