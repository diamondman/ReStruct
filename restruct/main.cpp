#include <iostream>
#include <fstream>

#include <restruct/restruct.hpp>

StructNode* int__StructConstructor
(std::string name, StructNodeRegistry* registry) {
  auto toString = registry->createScript
    ("return string.format('%d<uint32>', nodeval)");
  auto read = registry->createScript("return restruct.readuint32(rs)");
  auto node = new StructNodeLeaf(registry, name, toString, read);
  return node;
}

StructNode* rawStr__StructConstructor
(std::string name, StructNodeRegistry* registry) {
  auto toString = registry->createScript("return nodeval .. '<rawStr>'");
  auto read = registry->createScript("\
local inputs = restruct.getInputs(rs)\n\
return restruct.readString(rs, inputs.length)");
  auto node = new StructNodeLeaf(registry, name, toString, read);
  node->addInput("length");
  return node;
}

StructNode* lenStr__StructConstructor
(std::string name, StructNodeRegistry* registry) {
  auto toString = registry->createScript("\
local children = restruct.getChildrenValues(rs)\n\
return string.format('(\"%s\"; Len %d)<lenStr>', children.content, children.length)");
  auto node = new StructNodeGroup(registry, name, toString);
  node->addChild("<int>", "length");
  node->addChild("<rawstr>", "content");
  node->mapOutputToInput("length", "content", "length");
  return node;
}

StructNode* lenStrList__StructConstructor
(std::string name, StructNodeRegistry* registry) {
  auto toString = registry->createScript("return '(1 string)<>'");
  auto node = new StructNodeGroup(registry, name, toString);
  node->addChild("<lenstr>", "string0");
  node->addChild("<lenstr>", "string1");
  return node;
}

StructNode* TLTEST__StructConstructor
(std::string name, StructNodeRegistry* registry) {
  auto toString = registry->createScript("\
local children = restruct.getChildrenValues(rs)\n\
return string.format('(%d strings)<TLTEST>', children.length)");
  auto node = new StructNodeGroup(registry, name, toString);
  node->addChild("<int>", "length");
  node->addChild("<lenStrList>", "strings");
  return node;
}

int main(int argc, char** argv) {
  StructNodeRegistry nodeRegistry{};
  nodeRegistry.registerType("<int>",    &int__StructConstructor);
  nodeRegistry.registerType("<rawstr>", &rawStr__StructConstructor);
  nodeRegistry.registerType("<lenstr>", &lenStr__StructConstructor);
  nodeRegistry.registerType("<lenStrList>", &lenStrList__StructConstructor);
  nodeRegistry.registerType("<TLTEST>", &TLTEST__StructConstructor);

  std::ifstream myFile("test.bin", std::ios::in | std::ios::binary);
  auto sRoot = nodeRegistry.getNodeTypeByName("<TLTEST>");
  auto rRoot = sRoot->parseStream(myFile, "ROOT");
  rRoot->dumpTree();

  return 0;
}
