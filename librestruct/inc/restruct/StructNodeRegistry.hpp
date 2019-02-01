#pragma once

#include <map>
#include <memory>
#include <string>

class LuaScript;
class StructNode;
typedef struct lua_State lus_State;

class StructNodeRegistry {
  friend class LuaScript;

public:
  StructNodeRegistry();

  ~StructNodeRegistry();

  void registerType(std::string typeName,
                    StructNode* (*constructor)(std::string, StructNodeRegistry*));

  std::shared_ptr<StructNode> addNodeType(StructNode* type);

  std::shared_ptr<StructNode> removeNodeTypeByName(std::string name);

  std::shared_ptr<StructNode> getNodeTypeByName(std::string name);

  std::shared_ptr<LuaScript> createScript(std::string src);

  int registerScript(std::string src);

  void unRegisterScript(int id);

private:
  std::map<std::string, StructNode* (*)(std::string, StructNodeRegistry*)> typeConstructors;
  std::map<std::string, std::weak_ptr<StructNode>> nodeLookup;
public:
  lua_State *L;
};
