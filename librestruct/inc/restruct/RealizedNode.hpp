#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <vector>

class StructNode;

class RealizedNode : public std::enable_shared_from_this<RealizedNode> {
  friend class StructNodeGroup;

private:
  RealizedNode(std::shared_ptr<StructNode> structNode_,
               std::shared_ptr<RealizedNode> parent_,
               std::string nodeName_,
               std::istream& instream_);
  void finishInit();

public:
  ~RealizedNode();

  static std::shared_ptr<RealizedNode>
  create(std::shared_ptr<StructNode> structNode_,
         std::shared_ptr<RealizedNode> parent_,
         std::string nodeName_,
         std::istream& instream_);

  std::shared_ptr<RealizedNode> getParent() {
    return this->parent.lock();
  }

  std::shared_ptr<StructNode> getStructNode() {
    return this->structNode;
  }

  std::string getName() {
    return this->nodeName;
  }

  std::string getPath();

  void addChild(std::shared_ptr<RealizedNode> node) {
    this->children.push_back(node);
  }

  unsigned int getNumChildren() {
    return this->children.size();
  }

  std::shared_ptr<RealizedNode> getChild(unsigned int index) {
    if(index >= this->children.size()) return nullptr;
    return this->children[index];
  }

  std::string getValueAsString();

  void dumpTree(int depth=0);

private:
  std::shared_ptr<StructNode> structNode;
  std::weak_ptr<RealizedNode> parent;
  std::string nodeName;
public:
  std::istream& instream;
  int luaResultID=0;
private:
  std::streampos streamBeginOffset;
  std::streampos tmpOffset;

  std::vector<std::shared_ptr<RealizedNode>> children;
};
