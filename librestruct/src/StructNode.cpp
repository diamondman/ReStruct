#include <iostream>

#include <restruct/StructNode.hpp>
#include <restruct/RealizedNode.hpp>

std::shared_ptr<RealizedNode>
StructNode::parseStream(std::istream& instream,
                        std::string nodeName,
                        std::shared_ptr<RealizedNode> parent) {
  return RealizedNode::create(shared_from_this(), parent, nodeName, instream);
}

StructNode::~StructNode() {
  std::cerr << "******StructNode DEALLOCATING: " << this->name << std::endl;
}
