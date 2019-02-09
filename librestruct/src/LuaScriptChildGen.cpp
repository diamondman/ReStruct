#include <iostream>
#include <stdio.h>

#include "LuaScriptChildGen.hpp"

void LuaScriptChildGen::operator()(RealizedNode* node) {
  this->pcall(node, 0);
}
