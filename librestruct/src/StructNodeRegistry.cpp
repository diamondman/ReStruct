#include <cstring>
#include <string>
#include <iostream>
#include <iomanip> // For std::hex
#include <lua.h>
#include <lauxlib.h>

#include "LuaSandbox.hpp"
#include "LuaScriptChildGen.hpp"
#include "LuaScriptRead.hpp"
#include "LuaScriptToString.hpp"
#include <restruct/RealizedNode.hpp>
#include <restruct/StructNode.hpp>
#include <restruct/StructNodeDynGroup.hpp>
#include <restruct/StructNodeLeaf.hpp>
#include <restruct/StructNodeRegistry.hpp>

static RealizedNode *checkReStructNode (lua_State *L, int index,
                                        const char* type=NULL) {
  LuaScriptNodeContext* d;
  if(type) {
    d = (LuaScriptNodeContext*)luaL_checkudata(L, index, type);
  } else {
    d = (LuaScriptNodeContext*)lua_touserdata(L, index);
  }
  luaL_argcheck(L, d != NULL, index, type);
  luaL_argcheck(L, d->datum != NULL, index, "Script Context is null pointer");
  return d->datum;
}

static int RealizedNode_readString (lua_State *L) {
  RealizedNode* rNode = checkReStructNode
    (L, 1, "ReStruct.leafNodeReadStreamContext");

  int strlen = luaL_checkinteger(L, 2);
  luaL_argcheck(L, strlen > 0, 2, "`length' must be > 0");

  char *tmp = new char[strlen];
  rNode->instream.read(tmp, strlen);
  lua_pushlstring(L, (char*)tmp, strlen);  /* push result */
  delete[] tmp;
  return 1;  /* number of results */
}

static int RealizedNode_readuint32 (lua_State *L) {
  RealizedNode* rNode = checkReStructNode
    (L, 1, "ReStruct.leafNodeReadStreamContext");

  uint32_t res;
  rNode->instream.read(reinterpret_cast<char*>(&res), sizeof(res));
  lua_pushinteger(L, res);  /* push result */
  return 1;  /* number of results */
}

static int RealizedNode_getChildrenStrings (lua_State *L) {
  RealizedNode* rNode = checkReStructNode(L, 1, "ReStruct.nodeToStringContext");
  if(auto sNode = std::dynamic_pointer_cast<StructNodeLeaf>
     (rNode->getStructNode())) {
    lua_pushnil(L);
    return 1;
  }

  char* derp = (char*)malloc(122);

  lua_createtable(L, 0, rNode->getNumChildren());
  for(int i = 0; i < rNode->getNumChildren(); i++) {
    auto child = rNode->getChild(i);
    lua_pushstring(L, child->getName().c_str());
    if(child->getStructNode()->toStringScript) {
      (*child->getStructNode()->toStringScript)(child.get());
    } else {
      lua_pushnil(L);
    }
    lua_settable(L, -3);
  }
  return 1;  /* number of results */
}

static int RealizedNode_getChildrenValues (lua_State *L) {
  RealizedNode* rNode = checkReStructNode(L, 1, "ReStruct.nodeToStringContext");
  if(auto sNode = std::dynamic_pointer_cast<StructNodeLeaf>
     (rNode->getStructNode())) {
    lua_pushnil(L);
    return 1;
  }

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

static int RealizedNode_emitChild (lua_State *L) {
  RealizedNode* rNode = checkReStructNode
    (L, 1, "ReStruct.dynGroupNodeChildGenContext");

  auto sNode = rNode->getStructNode();
  std::cout << "Calling emit on sNode " << sNode->getName() << std::endl;
  std::shared_ptr<StructNodeDynGroup> sDynNode =
    std::dynamic_pointer_cast<StructNodeDynGroup>(sNode);
  if(!sDynNode) {
    std::cout << "Could not cast StructNode to StructNodeDynGroup" << std::endl;
    return 0;
  }
  sDynNode->emitChild(rNode->instream, rNode->shared_from_this());
  return 0;
}

static int RealizedNode_getValue(lua_State *L) {
  RealizedNode* rNode = checkReStructNode(L, 1, "ReStruct.nodeToStringContext");

  if(auto sNode = std::dynamic_pointer_cast<StructNodeGroup>
     (rNode->getStructNode())) {
    lua_pushnil(L);
  } else {
    lua_pushinteger(L, rNode->luaResultID);
    lua_gettable(L, LUA_REGISTRYINDEX);
  }
  return 1;
}

static int RealizedNode_getInputs (lua_State *L) {
  RealizedNode* rNode = checkReStructNode(L, 1);

  auto rParent = rNode->getParent();
  auto sParent = rParent->getStructNode();

  sParent->pushLuaInputsTable(L, rParent, rNode);
  return 1;
}

static int RealizedNode_log(lua_State *L) {
  RealizedNode* rNode = checkReStructNode(L, 1);
  const char *msg = luaL_checkstring(L, 2);
  std::cerr << "<LUA LOG>: " << msg << std::endl;
  return 0;
}

static int RealizedNode_getPath(lua_State *L) {
  RealizedNode* rNode = checkReStructNode(L, 1);

  std::stringstream stream;
  stream << std::hex << rNode;
  std::string result( stream.str() );

  std::string path = rNode->getPath() + " (" + result + ")";
  std::cout << "GOT PATH " << path << std::endl;
  lua_pushstring(L, path.c_str());
  return 1;
}

// ALL
static const struct luaL_Reg restructLib_common [] = {
  {"getInputs", RealizedNode_getInputs},
  {"log", RealizedNode_log},
  {"getPath", RealizedNode_getPath},
  {NULL, NULL}
};

// LEAF NODE READ
static const struct luaL_Reg restructLib_readStream [] = {
  {"readString", RealizedNode_readString},
  {"readuint32", RealizedNode_readuint32},
  {NULL, NULL}
};

// GROUP NODE TOSTRING, maybe ALL TOSTRING (but nil for leaf)
static const struct luaL_Reg restructLib_groupToStringFuncs [] = {
  {"getChildrenStrings", RealizedNode_getChildrenStrings},
  {"getChildrenValues", RealizedNode_getChildrenValues},
  {NULL, NULL}
};

// LEAF NODE TOSTRING, maybe ALL TOSTRING (but nil for group)
static const struct luaL_Reg restructLib_leafToStringFuncs [] = {
  {"getValue", RealizedNode_getValue},
  {NULL, NULL}
};

// DYN GROUP CHILD GEN
static const struct luaL_Reg restructLib_dynChildFuncs [] = {
  {"emitChild", RealizedNode_emitChild},
  {NULL, NULL}
};


static void createLuaType(lua_State* L, const char* name,
                          const struct luaL_Reg* funcs[]) {
  luaL_newmetatable(L, name);
  lua_pushstring(L, "__index");
  lua_pushvalue(L, -2);
  lua_settable(L, -3);

  for(const struct luaL_Reg** i = funcs; *i; i++){
    std::cout << "registering functions for type: " << name << std::endl;
    luaL_setfuncs(L, *i, 0);
  }

  lua_pop(L, 1); // Clear the metatable from the stack
}


StructNodeRegistry::StructNodeRegistry() {
  L = luaL_newstate();
  restruct_openlibs(L); /* Load Lua libraries */

  const struct luaL_Reg* leafNodeReadStreamContextFuncs[] =
    {restructLib_common, restructLib_readStream, NULL};
  createLuaType(L, "ReStruct.leafNodeReadStreamContext",
                leafNodeReadStreamContextFuncs);

  const struct luaL_Reg* nodeToStringContextFuncs[] =
    {restructLib_common, restructLib_leafToStringFuncs,
     restructLib_groupToStringFuncs, NULL};
  createLuaType(L, "ReStruct.nodeToStringContext",
                nodeToStringContextFuncs);

  const struct luaL_Reg* dynGroupNodeChildGenContextFuncs[] =
    {restructLib_common, restructLib_dynChildFuncs, NULL};
  createLuaType(L, "ReStruct.dynGroupNodeChildGenContext",
                dynGroupNodeChildGenContextFuncs);
}

StructNodeRegistry::~StructNodeRegistry() {
  std::cerr << "******STRUCTNODEREGISTRY DEALLOCATING " << std::endl;
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

std::shared_ptr<LuaScriptChildGen>
StructNodeRegistry::createChildGenScript(std::string src) {
  return std::make_shared<LuaScriptChildGen>(this, src);
}

std::shared_ptr<LuaScriptRead>
StructNodeRegistry::createReadScript(std::string src) {
  return std::make_shared<LuaScriptRead>(this, src);
}

std::shared_ptr<LuaScriptToString>
StructNodeRegistry::createToStringScript(std::string src) {
  return std::make_shared<LuaScriptToString>(this, src);
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

void printEntry(lua_State* L, int i) {
  RealizedNode* rNode = checkReStructNode(L, 1);
  fprintf(stderr, "ENTRY(%d): %s %p\n", i,
          rNode?rNode->getPath().c_str(): "NONE", rNode);
}

void StructNodeRegistry::basicStackDump(const char* suffix) {
  for(int i = lua_gettop(L); i != 0; i--)
    fprintf(stderr, "[%d]%s ", i, lua_typename(L, lua_type(L, i)));
  fprintf(stderr, "%s", suffix);
}

void StructNodeRegistry::stackDump () {
  int i=lua_gettop(L);
  fprintf(stderr," ----------------  Stack Dump (%d) ----------------\n**" , i);

  i=lua_gettop(L);
  while(  i   ) {
    int t = lua_type(L, i);
    switch (t) {
    case LUA_TSTRING:
      fprintf(stderr, "%d:`%s'\n", i, lua_tostring(L, i));
      break;
    case LUA_TBOOLEAN:
      fprintf(stderr, "%d: %s\n",i,lua_toboolean(L, i) ? "true" : "false");
      break;
    case LUA_TNUMBER:
      fprintf(stderr, "%d: %g\n",  i, lua_tonumber(L, i));
      break;
    case LUA_TFUNCTION:
      fprintf(stderr, "%d: FUNCTION\n",  i);
      break;
    case LUA_TTABLE:
      fprintf(stderr, "%d: TABLE\n",  i);
      break;
    default: {
      RealizedNode* rNode = checkReStructNode(L, i);
      int ret = lua_getmetatable(L, i);
      const char* name = 0;
      if (ret) {
        lua_pushstring(L, "__name");
        lua_gettable(L, -2);
        name = lua_tostring(L, -1);
      }

      fprintf(stderr, "%d: %s; %s; %s %p\n", i,
              lua_typename(L, t), name,
              rNode?rNode->getPath().c_str(): "NONE", rNode);
      if(ret)
        lua_pop(L, 2);
      break;
      }
    }
    i--;
  }
  fprintf(stderr, "--------------- Stack Dump Finished ---------------\n\n" );
}
