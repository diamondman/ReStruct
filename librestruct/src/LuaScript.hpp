#pragma once

#include <string>

class StructNodeRegistry;
class RealizedNode;

typedef struct LuaScriptNodeContext_s {
  RealizedNode* datum;
} LuaScriptNodeContext;

class LuaScript {
public:
  LuaScript(StructNodeRegistry* registry_, std::string src);
  ~LuaScript();

protected:
  void pushRealizedNode(RealizedNode* node, const char* type);
  void pcall(RealizedNode* node, const char* type, int nresults=1);
  void printTOS();

  StructNodeRegistry *registry;
  int scriptID;
};
