#include <iostream>
#include <restruct/LuaScript.hpp>
#include <restruct/RealizedNode.hpp>
#include <restruct/StructNodeLeaf.hpp>

StructNodeLeaf::StructNodeLeaf(StructNodeRegistry* registry_, std::string name_,
                               LuaScript* toStringScript_, LuaScript* readScript_) :
  StructNode(registry_, name_, toStringScript_), readScript(readScript_) {}

RealizedNode* StructNodeLeaf::parseStream(std::istream& instream,
                                          std::string nodeName,
                                          RealizedNode* parent) {
  RealizedNode* node = StructNode::parseStream(instream, nodeName, parent);
  this->readScript->pushRealizedNode("rs", node);
  this->readScript->call(&node->luaResultID);
  std::cout << "ret: " << this->toStringScript->calls(node, node->luaResultID)
            << std::endl;
  return node;
}
