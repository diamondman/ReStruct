#include <iostream>
#include <fstream>

#include <vector>

#include <restruct/restruct.hpp>

class RealizedNode;
class StructNode;

class RealizedNode {
public:
  RealizedNode(StructNode* structNode_, RealizedNode* parent_,
               std::istream& instream_);

  RealizedNode* getParent() {
    return this->parent;
  }

  StructNode* getStructNode() {
    return this->structNode;
  }

  void addChild(RealizedNode* node) {
    this->children.push_back(node);
  }

private:
  StructNode* structNode;
  RealizedNode* parent;
  std::istream& instream;
  std::streampos streamBeginOffset;

  std::vector<RealizedNode*> children;
};

//////////////////////////////////////////

class StructNodeLeaf;
class StructNodeGroup;

class StructNode {
public:
  StructNode(StructNodeGroup* parent_, std::string name_);

  StructNodeGroup* getParent() {
    return this->parent;
  }

  virtual unsigned int getNumChildren()=0;

  virtual StructNode* getChild(unsigned int index)=0;

  virtual RealizedNode* parseStream(std::istream& instream,
                                    RealizedNode* parent=nullptr) {
    return new RealizedNode(this, parent, instream);
  }

  std::string getName() {
    return this->name;
  }

protected:
  std::string name;
  StructNodeGroup* parent;
};

class StructNodeGroup : public StructNode {
public:
  StructNodeGroup(StructNodeGroup* parent_, std::string name_) :
    StructNode(parent_, name_) {

  }

  virtual unsigned int getNumChildren() {
    return this->children.size();
  }

  virtual StructNode* getChild(unsigned int index) {
    if(index >= this->children.size()) return nullptr;
    return this->children[index];
  }

  void addChild(StructNode* node) {
    this->children.push_back(node);
  }

  virtual RealizedNode* parseStream(std::istream& instream,
                                    RealizedNode* parent=nullptr) {
    RealizedNode* node = StructNode::parseStream(instream, parent);
    this->realizeChildren(instream, node);
    return node;
  }

private:
  void realizeChildren(std::istream& instream, RealizedNode* currNode) {
    for(auto it = children.begin(); it != children.end(); ++it) {
      (*it)->parseStream(instream, currNode);
    }
  }

private:
  std::vector<StructNode*> children;
};


StructNode::StructNode(StructNodeGroup* parent_, std::string name_) :
  parent(parent_), name(name_) {
  if(this->parent)
    this->parent->addChild(this);
}


class StructNodeLeaf : public StructNode {
public:
  StructNodeLeaf(StructNodeGroup* parent_, std::string name_) :
    StructNode(parent_, name_) {

  }

  virtual unsigned int getNumChildren() {
    return 0;
  }

  virtual StructNode* getChild(unsigned int index) {
    return nullptr;
  }
};

/////////////////////////////////////////////////

RealizedNode::RealizedNode(StructNode* structNode_, RealizedNode* parent_,
                           std::istream& instream_) :
  structNode(structNode_), parent(parent_), instream(instream_) {
  if(this->parent)
    this->parent->addChild(this);
  this->streamBeginOffset = this->instream.tellg();
  std::cout << "Realizing Node for " << this->structNode->getName() << std::endl;
}

///////////////////////////////////////////////////

int main(int argc, char** argv) {
  std::cout << "Hello" << std::endl;
  restruct_main(argc, argv);

  std::ifstream myFile("test.bin", std::ios::in | std::ios::binary);

  StructNodeGroup sRoot{nullptr, "ROOT"};
  StructNodeGroup sNames{&sRoot, "NAMES"};
  StructNodeLeaf  sNames_Len{&sNames, "NAMES LEN"};
  StructNodeGroup sNames_Entries{&sNames, "NAMES_ENTRIES"}; // FOR
  StructNodeGroup sStringComp{&sNames_Entries, "STRINGCOMP"};
  StructNodeLeaf  sString_Len{&sStringComp, "STR_LEN"};
  StructNodeLeaf  sString_Cstr{&sStringComp, "STR_CSTR"};

  RealizedNode *rRoot = sRoot.parseStream(myFile);

  delete rRoot;

  //int count;
  //myFile.read(reinterpret_cast<char*>(&count), sizeof(int));
  //std::cout << "Count: " << count << "; tell: " << myFile.tellg() << std::endl;

  //for(int i = 0; i < count; i++) {
  //  int strlen;
  //  myFile.read(reinterpret_cast<char*>(&strlen), sizeof(int));
  //  if(strlen) {
  //    char *s = new char[strlen];
  //    myFile.read(s, strlen);
  //    std::cout << "\"" << s << "\"; len: " << strlen << std::endl;
  //    delete[] s;
  //  } else {
  //    std::cout << "\"\"; len: 0" << std::endl;
  //  }
  //}
  return 0;
}
