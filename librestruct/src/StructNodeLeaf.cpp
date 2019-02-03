#include <iostream>
#include <lua.h>

#include <restruct/LuaScript.hpp>
#include <restruct/RealizedNode.hpp>
#include <restruct/StructNodeLeaf.hpp>

StructNodeLeaf::StructNodeLeaf(StructNodeRegistry* registry_, std::string name_,
                               std::shared_ptr<LuaScript> toStringScript_,
                               std::shared_ptr<LuaScript> readScript_) :
  StructNode(registry_, name_, toStringScript_), readScript(readScript_) {}

std::shared_ptr<RealizedNode>
StructNodeLeaf::parseStream(std::istream& instream,
                            std::string nodeName,
                            std::shared_ptr<RealizedNode> parent) {
  auto node = StructNode::parseStream(instream, nodeName, parent);
  this->readScript->pushRealizedNode("rs", node.get());
  this->readScript->call(&node->luaResultID);
  return node;
}

void StructNodeLeaf::pushLuaInputsTable(lua_State *L,
                                        std::shared_ptr<RealizedNode> realNode,
                                        RealizedNode* realChild) {
  std::cout << "\n\n\nHOW DID IT COME TO THIS\n\n\n" << std::endl;
  lua_pushnil(L);
}
