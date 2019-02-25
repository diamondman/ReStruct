#include <iostream>
#include <stdio.h>

#include "LuaScriptChildGen.hpp"
#include <restruct/RealizedNode.hpp>

void LuaScriptChildGen::operator()(RealizedNode* node) {
  std::cerr << "  Running Script " << node->getPath() << ".CHILDGEN" << std::endl;
  this->printTOS();
  this->pcall(node, "ReStruct.dynGroupNodeChildGenContext", 0);
  this->printTOS();
  std::cerr << "  Script END " << node->getPath() << ".CHILDGEN" << std::endl;
}
