#pragma once

#include <string>

#include <restruct/StructNode.hpp>

class StructNodeRegistry;

class StructNodeLeaf : public StructNode {
public:
  StructNodeLeaf(StructNodeRegistry* registry_, std::string name_,
                 std::shared_ptr<LuaScript> toStringScript_,
                 std::shared_ptr<LuaScript> readScript_);

  virtual unsigned int getNumChildren() {
    return 0;
  }

  virtual std::shared_ptr<RealizedNode>
  parseStream(std::istream& instream,
              std::string nodeName,
              std::shared_ptr<RealizedNode> parent);

  virtual void pushLuaInputsTable(lua_State *L,
                                  std::shared_ptr<RealizedNode> realNode,
                                  RealizedNode* realChild);

private:
  std::shared_ptr<LuaScript> readScript;
};
