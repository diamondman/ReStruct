#include <iostream>
#include <fstream>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <restruct/restruct.hpp>

StructNode* int__StructConstructor(std::string name, StructNodeRegistry* registry) {
  LuaScript* toString = new LuaScript(registry,
                                      //"return 'an int'");
  "return string.format('%d', nodeval) .. '<uint32>'");
  LuaScript* read = new LuaScript(registry, "return restruct.readuint32(rs)");
  return new StructNodeLeaf(registry, name, toString, read);
}

StructNode* rawStr__StructConstructor(std::string name,
                                      StructNodeRegistry* registry) {
  LuaScript* toString = new LuaScript(registry, "return 'DERP<rawStr>'");
  LuaScript* read = new LuaScript(registry, "return restruct.readString(rs, 3)");
  return new StructNodeLeaf(registry, name, toString, read);
}

StructNode* lenStr__StructConstructor(std::string name,
                                      StructNodeRegistry* registry) {
  LuaScript* toString = new LuaScript(registry, "return 'DERP<lenstr>'");
  auto ret = new StructNodeGroup(registry, name, toString);
  ret->addChild("<int>", "length");
  ret->addChild("<rawstr>", "content");
  return ret;
}

StructNode* lenStr__TLTEST(std::string name,
                           StructNodeRegistry* registry) {
  LuaScript* toString = nullptr;//new LuaScript(registry, "return '<TLTEST>'");
  auto ret = new StructNodeGroup(registry, name, 0);//toString);
  ret->addChild("<int>", "length1");
  ret->addChild("<int>", "length2");
  return ret;
}

void printRealizedNodes(RealizedNode* node, int depth=0) {
  for(int i = 0; i < depth; i++) std::cout << "  ";
  std::cout << node->getName() << ": " << node->getValueAsString() << std::endl;
  for(int i = 0; i < node->getNumChildren(); i++)
    printRealizedNodes(node->getChild(i), depth+1);
}

int main(int argc, char** argv) {
  //restruct_main(argc, argv);

  std::ifstream myFile("test.bin", std::ios::in | std::ios::binary);

  StructNodeRegistry nodeRegistry{};
  nodeRegistry.registerType("<int>",    &int__StructConstructor);
  nodeRegistry.registerType("<rawstr>", &rawStr__StructConstructor);
  nodeRegistry.registerType("<lenstr>", &lenStr__StructConstructor);
  nodeRegistry.registerType("<TLTEST>", &lenStr__TLTEST);

  auto sRoot = nodeRegistry.getNodeTypeByName("<TLTEST>");

  RealizedNode *rRoot = sRoot->parseStream(myFile, "ROOT");
  //rRoot->getChild(0)->getValueAsString();


  printRealizedNodes(rRoot);


  int s1 = nodeRegistry.registerScript("\
function test(a, b)\n\
  io.write(string.format('SCRIPT SAYS: %d, %d\\n\\n\\n', a, b))\n\
  return 1337\n\
end\n\
test(...)\n\
");

  lua_pushinteger(nodeRegistry.L, s1);  /* push address */
  lua_gettable(nodeRegistry.L, LUA_REGISTRYINDEX); // ToS = chunk to run
  lua_pushinteger(nodeRegistry.L, 45);  /* push arg0 */
  lua_pushinteger(nodeRegistry.L, 1337);  /* push arg1 */
  if (lua_pcall(nodeRegistry.L, 2, LUA_MULTRET, 0)) {
      fprintf(stderr, "Failed to run script: %s\n",
              lua_tostring(nodeRegistry.L, -1));
      exit(1);
  }
  int32_t ret = lua_tointeger(nodeRegistry.L, -1);
  printf("Script returned: 0x%x, %d\n", ret, ret);
  lua_pop(nodeRegistry.L, 1);  /* Take the returned value out of the stack */

  delete rRoot;

  return 0;
}
