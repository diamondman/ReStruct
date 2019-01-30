#pragma once

#include <fstream>
#include <string>
#include <vector>

#include <restruct/StructNode.hpp>
#include <restruct/StructNodeRegistry.hpp>

class RealizedNode;

class StructNodeGroup : public StructNode {
public:
  StructNodeGroup(StructNodeRegistry* registry_, std::string name_,
                  LuaScript* toStringScript_) :
    StructNode(registry_, name_, toStringScript_) {}

  virtual unsigned int getNumChildren() {
    return this->childrenNames.size();
  }

  //virtual std::shared_ptr<StructNode> getChild(unsigned int index) {
  //  if(index >= this->childrenNames.size()) return nullptr;
  //  std::string name = this->childrenNames[index];
  //  return nullptr;//.lock();
  //}

  void addChild(std::string nodeTypeName, std::string nodeName);

  virtual RealizedNode* parseStream(std::istream& instream,
                                    std::string nodeName,
                                    RealizedNode* parent=nullptr);

private:
  void realizeChildren(std::istream& instream, RealizedNode* currNode);

  // typename, nodename
  std::vector<std::pair<std::string, std::string>> childrenNames;
};
