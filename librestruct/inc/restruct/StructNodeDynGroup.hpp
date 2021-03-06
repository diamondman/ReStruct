#pragma once

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <restruct/StructNodeGroup.hpp>

class RealizedNode;
class LuaScriptChildGen;
class StructNodeRegistry;

class StructNodeDynGroup : public StructNodeGroup {
public:
  StructNodeDynGroup(StructNodeRegistry* registry_, std::string name_,
                     std::shared_ptr<LuaScriptToString> toStringScript_,
                     std::shared_ptr<LuaScriptChildGen> childGenScript_=nullptr) :
    StructNodeGroup(registry_, name_, toStringScript_),
    childGenScript(childGenScript_) {}

  virtual std::shared_ptr<RealizedNode>
  parseStream(std::istream& instream,
              std::string nodeName,
              std::shared_ptr<RealizedNode> parent);

  void emitChild(std::istream& instream,
                 std::shared_ptr<RealizedNode> currNode);

private:
  std::shared_ptr<LuaScriptChildGen> childGenScript;
};
