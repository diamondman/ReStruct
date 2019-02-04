#include <iostream>
#include <string>
#include <fstream>
#include <lua.h>

#include <restruct/StructNodeGroup.hpp>
#include <restruct/RealizedNode.hpp>

//class RealizedNode;

void StructNodeGroup::addChild(std::string nodeTypeName, std::string nodeName) {
  this->childrenNames.push_back(std::pair<std::string, std::string>(nodeTypeName, nodeName));
}

std::shared_ptr<RealizedNode>
StructNodeGroup::parseStream(std::istream& instream,
                             std::string nodeName,
                             std::shared_ptr<RealizedNode> parent) {
  auto node = StructNode::parseStream(instream, nodeName, parent);
  this->realizeChildren(instream, node);
  return node;
}

void StructNodeGroup::realizeChildren(std::istream& instream,
                                      std::shared_ptr<RealizedNode> currNode) {
  for(auto childInfo : this->childrenNames) {
    this->registry->getNodeTypeByName(childInfo.first)->
      parseStream(instream, childInfo.second, currNode);
  }
}

void StructNodeGroup::mapOutputToInput(std::string nodeNameFrom,
                                       std::string nodeNameTo,
                                       std::string inputName) {
  this->IOMap[nodeNameTo][inputName] = nodeNameFrom;
}

void StructNodeGroup::pushLuaInputsTable(lua_State *L,
                                         std::shared_ptr<RealizedNode> realNode,
                                         RealizedNode* realChild) {
  std::cerr << "  Pushing lua inputs table" << std::endl;
  lua_createtable(L, 0, this->inputs.size());

  if(this->IOMap.find(realChild->getName()) == this->IOMap.end()) {
    std::cerr << "  Node does not have values exported to it!" << std::endl;
    return;
  }

  std::cerr << "  Node has values exported to it!" << std::endl;
  auto inputMap = this->IOMap[realChild->getName()];

  for(auto input_it : realChild->getStructNode()->inputs) {
    std::string srcNodeName = inputMap[input_it];
    std::cerr << "  Exported from '" << srcNodeName << "' to '"
              << realChild->getName() << "." << input_it << "'" << std::endl;
    for(auto srcchild : realNode->children) {
      if(srcchild->getName() == srcNodeName) {
        std::cerr << "  NAME MATCH FOUND" << std::endl;
        lua_pushstring(L, input_it.c_str());

        lua_pushinteger(L, srcchild->luaResultID);
        lua_gettable(L, LUA_REGISTRYINDEX); // ToS = chunk to run

        lua_settable(L, -3);
        break;
      }
    }
  }
}
