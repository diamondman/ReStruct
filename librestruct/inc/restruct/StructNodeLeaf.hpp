#pragma once

#include <string>

#include <restruct/StructNode.hpp>

class StructNodeRegistry;

class StructNodeLeaf : public StructNode {
public:
  StructNodeLeaf(StructNodeRegistry* registry_, std::string name_,
                 LuaScript* toStringScript_, LuaScript* readScript_);

  virtual unsigned int getNumChildren() {
    return 0;
  }

  virtual std::shared_ptr<RealizedNode>
  parseStream(std::istream& instream,
              std::string nodeName,
              std::shared_ptr<RealizedNode> parent);

private:
  LuaScript* readScript;
};
