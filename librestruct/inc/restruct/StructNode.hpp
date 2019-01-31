#pragma once

#include <fstream>
#include <string>
#include <memory>

class LuaScript;
class RealizedNode;
class StructNodeRegistry;

class StructNode : public std::enable_shared_from_this<StructNode> {
public:
  StructNode(StructNodeRegistry* registry_, std::string name_,
             LuaScript* toStringScript_) :
    registry(registry_), name(name_), toStringScript(toStringScript_) {
    //this->registry->addNodeType(this);
    //if(this->parent)
    //  this->parent->addChild(this);
  }
  virtual ~StructNode();

  //StructNodeGroup* getParent() {
  //  return this->parent;
  //}

  virtual unsigned int getNumChildren()=0;

  //virtual std::shared_ptr<StructNode> getChild(unsigned int index)=0;

  virtual std::shared_ptr<RealizedNode>
  parseStream(std::istream& instream,
              std::string nodeName,
              std::shared_ptr<RealizedNode> parent=nullptr);

  std::string getName() {
    return this->name;
  }

protected:
  std::string name;
public:
  StructNodeRegistry* registry;
  LuaScript* toStringScript;
  //StructNodeGroup* parent;
};
