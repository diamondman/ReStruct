#pragma once

#include <fstream>
#include <string>
#include <memory>
#include <vector>

class LuaScript;
class RealizedNode;
class StructNodeRegistry;
typedef struct lua_State lus_State;

class StructNode : public std::enable_shared_from_this<StructNode> {
public:
  StructNode(StructNodeRegistry* registry_, std::string name_,
             std::shared_ptr<LuaScript> toStringScript_) :
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

  void addInput(std::string name) {
    this->inputs.push_back(name);
  }

  virtual void pushLuaInputsTable(lua_State *L,
                                  std::shared_ptr<RealizedNode> realNode,
                                  RealizedNode* realChild)=0;

protected:
  std::string name;
public:
  StructNodeRegistry* registry;
  std::shared_ptr<LuaScript> toStringScript;
  std::vector<std::string> inputs;
  //StructNodeGroup* parent;
};
