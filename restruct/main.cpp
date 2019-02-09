#include <iostream>
#include <fstream>

#include <restruct/restruct.hpp>

StructNode* int__StructConstructor
(std::string name, StructNodeRegistry* registry) {
  auto toString = registry->createScript("local rs = ...\n\
return string.format('%d<uint32>', rs:getValue())");
  auto read = registry->createScript("local rs = ...\n\
return rs:readuint32()");
  auto node = new StructNodeLeaf(registry, name, toString, read);
  return node;
}

StructNode* rawStr__StructConstructor
(std::string name, StructNodeRegistry* registry) {
  auto toString = registry->createScript("local rs = ...\n\
return rs:getValue() .. '<rawStr>'");
  auto read = registry->createScript("local rs = ...\n\
local inputs = rs:getInputs()\n\
return rs:readString(inputs.length)");
  auto node = new StructNodeLeaf(registry, name, toString, read);
  node->addInput("length");
  return node;
}

StructNode* lenStr__StructConstructor
(std::string name, StructNodeRegistry* registry) {
  auto toString = registry->createScript("local rs = ...\n\
local children = rs:getChildrenValues()\n\
return string.format('(\"%s\"; Len %d)<lenStr>', children.content, children.length)");
  auto node = new StructNodeGroup(registry, name, toString);
  node->addChild("<int>", "length");
  node->addChild("<rawstr>", "content");
  node->mapOutputToInput("length", "content", "length");
  return node;
}

StructNode* lenStrList__StructConstructor
(std::string name, StructNodeRegistry* registry) {
  auto toString = registry->createScript("local rs = ...\n\
local inputs = rs:getInputs()\n\
return string.format('(%d strings)<lenStr[]>', inputs.count)");
auto childGen = registry->createScript("local rs = ...\n\
local inputs = rs:getInputs()\n\
rs:log(string.format('EMITING %d ENTRIES\\b', inputs.count))\n\
for i=1,inputs.count,1 do\n\
   rs:log(string.format('    EMIT %d!\\n', i))\n    \
   rs:emitChild()\n\
end");
  auto node = new StructNodeDynGroup(registry, name, toString, childGen);
  node->addInput("count");
  node->addChild("<lenstr>", "str");
  return node;
}

StructNode* TLTEST__StructConstructor
(std::string name, StructNodeRegistry* registry) {
  auto toString = registry->createScript("local rs = ...\n\
local children = rs:getChildrenValues()\n\
return string.format('(%d strings)<TLTEST>', children.count)");
  auto node = new StructNodeGroup(registry, name, toString);
  node->addChild("<int>", "count");
  node->addChild("<lenStrList>", "strings");
  node->mapOutputToInput("count", "strings", "count");
  return node;
}

int main(int argc, char** argv) {
  StructNodeRegistry nodeRegistry{};
  nodeRegistry.registerType("<int>",    &int__StructConstructor);
  nodeRegistry.registerType("<rawstr>", &rawStr__StructConstructor);
  nodeRegistry.registerType("<lenstr>", &lenStr__StructConstructor);
  nodeRegistry.registerType("<lenStrList>", &lenStrList__StructConstructor);
  nodeRegistry.registerType("TLTEST", &TLTEST__StructConstructor);

  std::ifstream myFile("test.bin", std::ios::in | std::ios::binary);
  auto sRoot = nodeRegistry.getNodeTypeByName("TLTEST");
  auto rRoot = sRoot->parseStream(myFile, "ROOT");
  rRoot->dumpTree();

  return 0;
}
