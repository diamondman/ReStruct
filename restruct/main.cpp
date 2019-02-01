#include <iostream>
#include <fstream>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <restruct/restruct.hpp>

StructNode* int__StructConstructor(std::string name, StructNodeRegistry* registry) {
  auto toString = registry->createScript
    ("return string.format('%d<uint32>', nodeval)");
  auto read = registry->createScript("return restruct.readuint32(rs)");
  return new StructNodeLeaf(registry, name, toString, read);
}

StructNode* rawStr__StructConstructor(std::string name,
                                      StructNodeRegistry* registry) {
  auto toString = registry->createScript("return nodeval .. '<rawStr>'");
  auto read = registry->createScript("return restruct.readString(rs, 3)");
  return new StructNodeLeaf(registry, name, toString, read);
}

StructNode* lenStr__StructConstructor(std::string name,
                                      StructNodeRegistry* registry) {
  auto toString = registry->createScript("\
local children = restruct.getChildrenValues(rs)\n\
return string.format('(\"%s\"; Len %d)<lenStr>', children.content, children.length)");
  auto ret = new StructNodeGroup(registry, name, toString);
  ret->addChild("<int>", "length");
  ret->addChild("<rawstr>", "content");
  return ret;
}

StructNode* lenStrList__StructConstructor(std::string name,
                           StructNodeRegistry* registry) {
  auto toString = registry->createScript("return '(1 string)<>'");
  auto ret = new StructNodeGroup(registry, name, toString);
  ret->addChild("<lenstr>", "string0");
  ret->addChild("<lenstr>", "string1");
  return ret;
}

StructNode* TLTEST__StructConstructor(std::string name,
                           StructNodeRegistry* registry) {
  auto toString = registry->createScript("\
local children = restruct.getChildrenValues(rs)\n\
return string.format('(%d strings)<TLTEST>', children.length)");
  auto ret = new StructNodeGroup(registry, name, toString);
  ret->addChild("<int>", "length");
  ret->addChild("<lenStrList>", "strings");
  return ret;
}

int main(int argc, char** argv) {
  //restruct_main(argc, argv);

  std::ifstream myFile("test.bin", std::ios::in | std::ios::binary);

  StructNodeRegistry nodeRegistry{};
  nodeRegistry.registerType("<int>",    &int__StructConstructor);
  nodeRegistry.registerType("<rawstr>", &rawStr__StructConstructor);
  nodeRegistry.registerType("<lenstr>", &lenStr__StructConstructor);
  nodeRegistry.registerType("<lenStrList>", &lenStrList__StructConstructor);
  nodeRegistry.registerType("<TLTEST>", &TLTEST__StructConstructor);

  auto sRoot = nodeRegistry.getNodeTypeByName("<TLTEST>");

  auto rRoot = sRoot->parseStream(myFile, "ROOT");
  rRoot->dumpTree();


  int s1 = nodeRegistry.registerScript("\
function test(a, b)\n\
  print(string.format('SCRIPT SAYS: %d, %d\\n\\n\\n', a, b))\n\
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

  return 0;
}
