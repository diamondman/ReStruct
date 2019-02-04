#pragma once

#include <string>

class StructNodeRegistry;
class RealizedNode;

class LuaScript {
public:
  LuaScript(StructNodeRegistry* registry_, std::string src);
  ~LuaScript();

  void call(RealizedNode* node);

  void calls_luares(RealizedNode* node);

  std::string calls(RealizedNode* node);

  void callz(RealizedNode* node);

protected:
  void pushRealizedNode(RealizedNode* node);

private:
  StructNodeRegistry *registry;
  int scriptID;
};
