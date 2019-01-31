#include <iostream>
#include <restruct/LuaScript.hpp>
#include <restruct/RealizedNode.hpp>
#include <restruct/StructNodeLeaf.hpp>

StructNodeLeaf::StructNodeLeaf(StructNodeRegistry* registry_, std::string name_,
                               LuaScript* toStringScript_, LuaScript* readScript_) :
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
