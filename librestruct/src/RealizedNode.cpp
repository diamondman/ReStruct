#include <iostream>

#include <restruct/LuaScript.hpp>
#include <restruct/RealizedNode.hpp>
#include <restruct/StructNode.hpp>
#include <restruct/StructNodeRegistry.hpp>

std::shared_ptr<RealizedNode>
RealizedNode::create(std::shared_ptr<StructNode> structNode,
                     std::shared_ptr<RealizedNode> parent,
                     std::string nodeName,
                     std::istream& instream) {
  std::shared_ptr<RealizedNode> ret(new RealizedNode(structNode, parent,
                                                     nodeName, instream));
  ret->finishInit();
  return ret;
}

RealizedNode::RealizedNode(std::shared_ptr<StructNode> structNode_,
                           std::shared_ptr<RealizedNode> parent_,
                           std::string nodeName_,
                           std::istream& instream_) :
  structNode(structNode_), parent(parent_), nodeName(nodeName_),
  instream(instream_) {
  this->streamBeginOffset = this->instream.tellg();
  std::cout << "Realizing Node for " << this->structNode->getName()
            << "; Node name: '" << this->nodeName << "'" << std::endl;
}

void RealizedNode::finishInit() {
  if(auto p = this->parent.lock())
    p->addChild(shared_from_this());
}

RealizedNode::~RealizedNode() {
  std::cerr << "******RealizedNode DEALLOCATING " << this->nodeName << std::endl;
}

std::string RealizedNode::getValueAsString() {
  if(!this->structNode->toStringScript)
    return "...";
  return this->structNode->toStringScript->calls(this);
}

void RealizedNode::dumpTree(int depth) {
  for(int i = 0; i < depth; i++) std::cout << "  ";
  std::cout << this->getName() << ": " << this->getValueAsString() << std::endl;
  for(auto child : this->children)
    child->dumpTree(depth+1);
}
