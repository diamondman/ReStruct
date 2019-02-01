#include <iostream>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <restruct/restruct.hpp>



static const luaL_Reg loadedlibs[] = {
  {"_G", luaopen_base},
  //{LUA_LOADLIBNAME, luaopen_package},
  //{LUA_COLIBNAME, luaopen_coroutine},
  {LUA_TABLIBNAME, luaopen_table},
  //{LUA_IOLIBNAME, luaopen_io},
  //{LUA_OSLIBNAME, luaopen_os},
  {LUA_STRLIBNAME, luaopen_string},
  {LUA_MATHLIBNAME, luaopen_math},
  {LUA_UTF8LIBNAME, luaopen_utf8},
  //{LUA_DBLIBNAME, luaopen_debug},
  {NULL, NULL}
};


LUALIB_API void restruct_openlibs (lua_State *L) {
  const luaL_Reg *lib;
  /* "require" functions from 'loadedlibs' and set results to global table */
  for (lib = loadedlibs; lib->func; lib++) {
    luaL_requiref(L, lib->name, lib->func, 1);
    lua_pop(L, 1);  /* remove lib */
  }
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
#include <stdlib.h>
static int RealizedNode_getChildrenStrings (lua_State *L) {
  RealizedNode *rNode = (RealizedNode *)lua_touserdata(L, 1);
  luaL_argcheck(L, rNode != NULL, 1, "`RealizedNode' expected");

  char* derp = (char*)malloc(122);

  lua_createtable(L, 0, rNode->getNumChildren());
  for(int i = 0; i < rNode->getNumChildren(); i++) {
    auto child = rNode->getChild(i);
    lua_pushstring(L, child->getName().c_str());
    if(child->getStructNode()->toStringScript) {
      child->getStructNode()->toStringScript->calls_luares(child.get(),
                                                           child->luaResultID);
    } else {
      lua_pushnil(L);
    }
    lua_settable(L, -3);
  }
  return 1;  /* number of results */
}

static int RealizedNode_getChildrenValues (lua_State *L) {
  RealizedNode *rNode = (RealizedNode *)lua_touserdata(L, 1);
  luaL_argcheck(L, rNode != NULL, 1, "`RealizedNode' expected");

  lua_createtable(L, 0, rNode->getNumChildren());
  for(int i = 0; i < rNode->getNumChildren(); i++) {
    auto child = rNode->getChild(i);
    lua_pushstring(L, child->getName().c_str());
    if(child->luaResultID) {
      lua_pushinteger(L, child->luaResultID);
      lua_gettable(L, LUA_REGISTRYINDEX);
    } else {
      lua_pushnil(L);
    }
    lua_settable(L, -3);
  }
  return 1;  /* number of results */
}

#define luaL_reg      luaL_Reg
static const struct luaL_reg restructLib [] = {
  {"readString", RealizedNode_readString},
  {"readuint32", RealizedNode_readuint32},
  {"getChildrenStrings", RealizedNode_getChildrenStrings},
  {"getChildrenValues", RealizedNode_getChildrenValues},
  {NULL, NULL}
};


StructNodeRegistry::StructNodeRegistry() {
  L = luaL_newstate();
  restruct_openlibs(L); /* Load Lua libraries */

  if (luaL_loadstring(L, "for k,v in pairs(_G) do\n"
                         "  print('Global key', k, 'value', v)\n"
                         "end")) {
    fprintf(stderr, "Couldn't load source: %s\n", lua_tostring(L, -1));
    exit(1);
  }
  if (lua_pcall(L, 0, 0, 0)) {
      fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
      exit(1);
  }

  lua_newtable(L);
  luaL_setfuncs(L, restructLib, 0);
  lua_setglobal(L, "restruct");
}

StructNodeRegistry::~StructNodeRegistry() {
  std::cout << "******STRUCTNODEREGISTRY DEALLOCATING " << std::endl;
  lua_close(this->L);
}

void StructNodeRegistry::registerType
(std::string typeName, StructNode*(*constructor)(std::string, StructNodeRegistry*)) {
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

std::shared_ptr<LuaScript> StructNodeRegistry::createScript(std::string src) {
  return std::make_shared<LuaScript>(this, src);
}

int StructNodeRegistry::registerScript(std::string src) {
  if (luaL_loadstring(L, src.c_str())) {
    fprintf(stderr, "Couldn't load source: %s\n", lua_tostring(L, -1));
    exit(1);
  }
  return luaL_ref(L, LUA_REGISTRYINDEX);
}

void StructNodeRegistry::unRegisterScript(int id) {
  luaL_unref(L, LUA_REGISTRYINDEX, id);
}
