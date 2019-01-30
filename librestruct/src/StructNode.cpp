#include <iostream>

#include <restruct/StructNode.hpp>
#include <restruct/RealizedNode.hpp>

RealizedNode* StructNode::parseStream(std::istream& instream,
                                      std::string nodeName,
                                      RealizedNode* parent) {
  return new RealizedNode(shared_from_this(), parent, nodeName, instream);
}

StructNode::~StructNode() {
  std::cout << "******StructNode DEALLOCATING: " << this->name << std::endl;
}
