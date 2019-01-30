#include <iostream>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <restruct/restruct.hpp>

#include <math.h>
static int l_sin (lua_State *L) {
  double d = luaL_checknumber(L, 1);
  lua_pushnumber(L, sin(d));  /* push result */
  return 1;  /* number of results */
}


static int RealizedNode_readString (lua_State *L) {
  RealizedNode *rNode = (RealizedNode *)lua_touserdata(L, 1);
  luaL_argcheck(L, rNode != NULL, 1, "`RealizedNode' expected");
  int strlen = luaL_checkinteger(L, 2);
  luaL_argcheck(L, strlen > 0, 2, "`length' must be > 0");

  char *tmp = new char[strlen];
  rNode->instream.read(tmp, strlen);
  lua_pushlstring(L, (char*)tmp, strlen);  /* push result */
  delete[] tmp;
  return 1;  /* number of results */
}

static int RealizedNode_readuint32 (lua_State *L) {
  RealizedNode *rNode = (RealizedNode *)lua_touserdata(L, 1);
  luaL_argcheck(L, rNode != NULL, 1, "`RealizedNode' expected");

  uint32_t res;
  rNode->instream.read(reinterpret_cast<char*>(&res), sizeof(res));
  lua_pushinteger(L, res);  /* push result */
  return 1;  /* number of results */
}

#define luaL_reg      luaL_Reg
static const struct luaL_reg restructLib [] = {
  {"readString", RealizedNode_readString},
  {"readuint32", RealizedNode_readuint32},
  {NULL, NULL}
};


StructNodeRegistry::StructNodeRegistry() {
  L = luaL_newstate();
  luaL_openlibs(L); /* Load Lua libraries */

  lua_pushcfunction(L, l_sin);
  lua_setglobal(L, "mysin");

  lua_newtable(L);
  luaL_setfuncs(L, restructLib, 0);
  lua_setglobal(L, "restruct");
}

void StructNodeRegistry::registerType(std::string typeName,
                  StructNode* (*constructor)(std::string, StructNodeRegistry*)) {
  this->typeConstructors[typeName] = constructor;
}

std::shared_ptr<StructNode> StructNodeRegistry::addNodeType(StructNode* type) {
  std::cout << "Loading Registered StructNode type: \"" << type->getName()
            << "\"" << std::endl;
  std::shared_ptr<StructNode> sp(type);

  this->nodeLookup[type->getName()] = sp;
  return sp;
}

std::shared_ptr<StructNode>
StructNodeRegistry::removeNodeTypeByName(std::string name) {
  std::shared_ptr<StructNode> res(nullptr);
  auto it = this->nodeLookup.find(name);
  if(it != this->nodeLookup.end()) {
    res = it->second.lock();
    this->nodeLookup.erase(it);
  }
  return res;
}

std::shared_ptr<StructNode>
StructNodeRegistry::getNodeTypeByName(std::string name) {
  {
    auto it = this->nodeLookup.find(name);
    if(it != this->nodeLookup.end()) {
      if(std::shared_ptr<StructNode> res = it->second.lock()) {
        return res;
      }
    }
  }
  {
    auto it = this->typeConstructors.find(name);
    if(it != this->typeConstructors.end()) {
      auto constructor = it->second;
      return this->addNodeType(constructor(name, this));
    }
  }

  return nullptr;
}

int StructNodeRegistry::registerScript(std::string src) {
  if (luaL_loadstring(L, src.c_str())) {
    fprintf(stderr, "Couldn't load source: %s\n", lua_tostring(L, -1));
    exit(1);
  }
  return luaL_ref(L, LUA_REGISTRYINDEX);
}
