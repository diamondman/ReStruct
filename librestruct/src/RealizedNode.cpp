#include <iostream>

#include <restruct/LuaScript.hpp>
#include <restruct/RealizedNode.hpp>
#include <restruct/StructNode.hpp>
#include <restruct/StructNodeRegistry.hpp>

RealizedNode::RealizedNode(std::shared_ptr<StructNode> structNode_,
                           RealizedNode* parent_, std::string nodeName_,
                           std::istream& instream_) :
  structNode(structNode_), parent(parent_), nodeName(nodeName_),
  instream(instream_) {
  if(this->parent)
    this->parent->addChild(this);
  this->streamBeginOffset = this->instream.tellg();
  std::cout << "Realizing Node for " << this->structNode->getName()
            << "; Node name: '" << this->nodeName << "'" << std::endl;
}

RealizedNode::~RealizedNode() {
  std::cout << "******RealizedNode DEALLOCATING" << std::endl;
}

std::string RealizedNode::getValueAsString() {
  if(this->structNode->toStringScript)
    //this->structNode->toStringScript->call(&this->luaResultID);
    return this->structNode->toStringScript->calls(this, this->luaResultID);
  return "...";
}
