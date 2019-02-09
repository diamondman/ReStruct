#pragma once

#include <string>

class StructNodeRegistry;
class RealizedNode;

class LuaScript {
public:
  LuaScript(StructNodeRegistry* registry_, std::string src);
  ~LuaScript();

protected:
  void pushRealizedNode(RealizedNode* node);
  void pcall(RealizedNode* node, int nresults=1);

  StructNodeRegistry *registry;
  int scriptID;
};
