#pragma once

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <restruct/StructNode.hpp>

class RealizedNode;
class StructNodeRegistry;

class StructNodeGroup : public StructNode {
public:
  StructNodeGroup(StructNodeRegistry* registry_, std::string name_,
                  std::shared_ptr<LuaScriptToString> toStringScript_) :
    StructNode(registry_, name_, toStringScript_) {}

  virtual unsigned int getNumChildren() {
    return this->childrenNames.size();
  }

  void addChild(std::string nodeTypeName, std::string nodeName);

  virtual std::shared_ptr<RealizedNode>
  parseStream(std::istream& instream,
              std::string nodeName,
              std::shared_ptr<RealizedNode> parent=nullptr);



  void mapOutputToInput(std::string nodeNameFrom,
                        std::string nodeNameTo,
                        std::string inputName);

  virtual void pushLuaInputsTable(lua_State *L,
                                  std::shared_ptr<RealizedNode> realNode,
                                  RealizedNode* realChild);

private:
  void realizeChildren(std::istream& instream,
                       std::shared_ptr<RealizedNode> currNode);

protected:
  // typename, nodename
  std::vector<std::pair<std::string, std::string>> childrenNames;

  // nodeNameTo: {inputName: NodeNameFrom}
  std::map<std::string, std::map<std::string, std::string>> IOMap;
};
