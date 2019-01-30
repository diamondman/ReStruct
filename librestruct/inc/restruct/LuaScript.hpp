#pragma once

#include <string>

class StructNodeRegistry;
class RealizedNode;

class LuaScript {
public:
  LuaScript(StructNodeRegistry* registry_, std::string src);
  ~LuaScript();

  void pushIntParam(const char* name, int param);

  void pushNumberParam(const char* name, double param);

  void pushStringParam(const char* name, const void* str, int strlen = -1);

  void pushRealizedNode(const char* name, RealizedNode* node);

  void call(int* resultID);

  void calls_luares(RealizedNode* node, int luaResultID);

  std::string calls(RealizedNode* node, int luaResultID);

private:
  StructNodeRegistry *registry;
  int scriptID;
};
