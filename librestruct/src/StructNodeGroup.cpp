
#include <string>
#include <fstream>

#include <restruct/StructNodeGroup.hpp>

class RealizedNode;

void StructNodeGroup::addChild(std::string nodeTypeName, std::string nodeName) {
  this->childrenNames.push_back(std::pair<std::string, std::string>(nodeTypeName, nodeName));
}

RealizedNode* StructNodeGroup::parseStream(std::istream& instream,
                                           std::string nodeName,
                                           RealizedNode* parent) {
  RealizedNode* node = StructNode::parseStream(instream, nodeName, parent);
  this->realizeChildren(instream, node);
  return node;
}

void StructNodeGroup::realizeChildren(std::istream& instream,
                                      RealizedNode* currNode) {
  for(auto childInfo : this->childrenNames) {
    this->registry->getNodeTypeByName(childInfo.first)->
      parseStream(instream, childInfo.second, currNode);
  }
}
