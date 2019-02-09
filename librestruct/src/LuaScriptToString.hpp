#pragma once

#include <string>

#include "LuaScript.hpp"

class RealizedNode;

class LuaScriptToString : private LuaScript {
public:
  LuaScriptToString(StructNodeRegistry* registry_, std::string src) :
    LuaScript(registry_, src) {};
  std::string operator()(RealizedNode* node);
  void call_luares(RealizedNode* node);
};
