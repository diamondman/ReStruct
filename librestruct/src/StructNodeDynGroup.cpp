#include <iostream>
#include <string>
#include <fstream>
#include <lua.h>

#include <restruct/StructNodeDynGroup.hpp>
#include <restruct/LuaScript.hpp>

class RealizedNode;

std::shared_ptr<RealizedNode>
StructNodeDynGroup::parseStream(std::istream& instream,
                                std::string nodeName,
                                std::shared_ptr<RealizedNode> parent) {
  auto node = StructNode::parseStream(instream, nodeName, parent);
  std::cout << "CALLING CHILDGENSCRIPT" << std::endl;
  std::cout << "childgen CALL START! Stack top: " << lua_gettop(this->registry->L)
            << std::endl;
  this->childGenScript->pushRealizedNode("rs", node.get());
  this->childGenScript->callz(node.get());
  std::cout << "childgen CALL END! Stack top: " << lua_gettop(this->registry->L)
            << std::endl;
  return node;
}

void StructNodeDynGroup::emitChild(std::istream& instream,
                                   std::shared_ptr<RealizedNode> currNode) {
  if(this->childrenNames.size() == 0) return;
  auto childInfo = this->childrenNames[0];
  this->registry->getNodeTypeByName(childInfo.first)->
    parseStream(instream, childInfo.second, currNode);
}