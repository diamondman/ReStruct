#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <vector>

class StructNode;

class RealizedNode {
public:
  RealizedNode(std::shared_ptr<StructNode> structNode_,
               RealizedNode* parent_, std::string nodeName_,
               std::istream& instream_);

  ~RealizedNode();

  RealizedNode* getParent() {
    return this->parent;
  }

  std::shared_ptr<StructNode> getStructNode() {
    return this->structNode;
  }

  std::string getName() {
    return this->nodeName;
  }

  void addChild(RealizedNode* node) {
    this->children.push_back(node);
  }

  unsigned int getNumChildren() {
    return this->children.size();
  }

  RealizedNode* getChild(unsigned int index) {
    if(index >= this->children.size()) return nullptr;
    return this->children[index];
  }

  std::string getValueAsString();

  void dumpTree(int depth=0);

private:
  std::shared_ptr<StructNode> structNode;
  RealizedNode* parent;
  std::string nodeName;
public:
  std::istream& instream;
  int luaResultID;
private:
  std::streampos streamBeginOffset;
  std::streampos tmpOffset;

  std::vector<RealizedNode*> children;
};
