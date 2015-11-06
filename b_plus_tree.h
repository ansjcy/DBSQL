// B+ Tree
// Bohan Li

#ifndef B_PLUS_TREE
#define B_PLUS_TREE

#include <iostream>
#include <cstdlib>
#include <vector>
#include <stack>
#include <iterator>
#include <algorithm>
#include <string>
#include <map>

#include "recorder.hpp"

#define BLOCK_SIZE 4096

#define TEMP_FANOUT 100

class Block {
public:
    int pos;
    char* data;
    std::string filename;
    
    Block() {
        data = (char*)malloc(BLOCK_SIZE);
    };
    Block(int _pos) { pos = _pos; }
    void setPos(int _pos) { pos = _pos; }
    int getPos() { return pos; }
};



class BufferManager {
public:
    recorder* r;
    
    std::string fname;
    
    void setRecorder(recorder* _r) { r = _r; }
    void getBlock(std::string filename, int offset, Block& blk){
        std::vector<int> offsetV;
        offsetV.push_back(offset);
        conditionJudge cJ;
        
        std::cout << "getBlock" << " Offset = " << offset/BLOCK_SIZE << std::endl;
        
        vector<void*> rst = r->selectWhereIndex(filename, offsetV, cJ);
        
        char* mem = (char*)rst[0];
        
        memcpy(blk.data, mem, BLOCK_SIZE);
        blk.pos = offset;
        
        
    }
    
    Block newBlock(std::string filename){
        char* nulldata = (char*)malloc(BLOCK_SIZE);
        for( int i = 0; i < BLOCK_SIZE; i++ ) {
            nulldata[i] = 'A';
        }
        long pos = r->insert(nulldata, filename);
        Block blk;
        blk.pos = pos;
        blk.filename = filename;
        return blk;
    };
    
    void writeBlock(Block& block) {
        std::cout << "writeBlock -- block.pos" << block.pos/BLOCK_SIZE << std::endl;
        char* mem = block.data;
        r->updateWhere(fname, block.pos, block.data);
    }
    
    void deleteBlock(Block block) {
        conditionJudge cJ;
        r->deleteWhereIndex(block.filename, block.pos, cJ);
    }
    recorder* getRecorder() { return r; }
};


class AttrType {
public:
    int typeId;
    int datai;
    float dataf;
    std::string datas;
    AttrType() {}
    AttrType(int i): datai(i), typeId(0) {}
    AttrType(float f): dataf(f), typeId(1)  {}
    AttrType(char s[]): datas(std::string(s)), typeId(2)  {}
    AttrType(std::string s): datas(s), typeId(2)  {}
    
    friend bool operator<(const AttrType &op1, const AttrType &op2) {
        if( op1.typeId == 0 ) return op1.datai < op2.datai;
        if( op1.typeId == 1 ) return op1.dataf < op2.dataf;
        if( op1.typeId == 2 ) return op1.datas < op2.datas;
        
        return true;
    }
    
    friend bool operator==(const AttrType &op1, const AttrType &op2) {
        if( op1.typeId == 0 ) return op1.datai == op2.datai;
        if( op1.typeId == 1 ) return op1.dataf == op2.dataf;
        if( op1.typeId == 2 ) return op1.datas == op2.datas;
        
        return true;
    }
    
    friend bool operator>(const AttrType &op1, const AttrType &op2) { return !(op1 == op2 || op1 < op2); }
    friend bool operator<=(const AttrType &op1, const AttrType &op2) { return !( op1 > op2); }
    friend bool operator>=(const AttrType &op1, const AttrType &op2) { return !( op1 < op2); }
    friend bool operator!=(const AttrType &op1, const AttrType &op2) { return !( op1 == op2); }
    
};

class Node {
private:
    bool isroot, isleaf;
    
    int readi(char *&mem);
    float readf(char *&mem);
    std::string reads(char *&mem, int strLen);
    void writei(int i, char *&mem);
    void writef(float f, char *&mem);
    void writes(std::string s, int strLen, char *&mem );
    
public:
    std::vector<int> P;
    std::vector<AttrType> K;
    
    void resetByBlock(const Block &blk);
    Node();
    Node(bool _isroot, bool _isleaf);
    Node(const Block &blk);
    void write2Blk(Block &blk, int typeId, int strLen, BufferManager* bufferManager);
    bool isRoot();
    bool isLeaf();
    void setRoot(bool _isroot) {isroot = _isroot; }
    void setLeaf(bool _isleaf) {isleaf = _isleaf; }
    void appendK(const AttrType &_k);
    void appendP(int _p);
    void popK();
    void popP();
    int getKSize();
    int getPSize();
    AttrType getK(int i);
    int getP(int i);
    void insert(const AttrType &k, int p, int i);
    void remove(const AttrType &k, int p, int i);
};




class BPlusTree {
public:
    std::string filename; // {tableName}_{indexName}.idx
    Node rootNode;
    int rootPos;
    int fanout;
    int typeId, strLen;
    std::stack<int> stk;
    //BufferManager bufferManager;
    int _findLeaf(const AttrType &k);
    void _insertNewBlk(Block &blk1, const AttrType &k, Block &blk2);
    void _remove(Block blk, AttrType k);
    
    
    BufferManager* bufferManager;
    
    BPlusTree();
    BPlusTree(const std::string &_filename, int _typeId = 0, int _strLen = 0) : filename(_filename), typeId(_typeId), strLen(_strLen) {
        //
        
        
        // fanout = TEMP_FANOUT;
        
        fanout = (BLOCK_SIZE - 2 - 4 * 4 - 4) / (4+4) - 10;
        
        if( _typeId == 2 )
            fanout = (BLOCK_SIZE - 2 - 4 * 4 - 4) / (_strLen+1+4) - 10;
        
    }
    
    void setBufferManager(BufferManager* _bufferManager) { bufferManager = _bufferManager; }
    
    void setFilename(std::string _filename) { filename = _filename; }
    
    void setTypeId(int i) { typeId = i; };
    void setStrLen(int i) { strLen = i; };
    void setRootNode(Node n);
    void setRootPos(int p);
    int getTypeId() { return typeId; }
    int getStrLen() { return strLen; }
    
    int find(const AttrType &k);
    void insert(const AttrType &k, int p);
    void remove(const AttrType &k);
    //	std::vector<int> findLeft(const AttrType &k, bool (*cmp)(const AttrType &a,const AttrType &b));
    //	std::vector<int> findRight(const AttrType &k, bool (*cmp)(const AttrType &a,const AttrType &b));
    std::vector<int> findLeft(const AttrType &k);
    std::vector<int> findRight(const AttrType &k);
};

class IndexManager {
public:
    //BufferManager bfManager;
    BPlusTree btree;
    std::string filename;
    
    BufferManager bufferManager;
    
    IndexManager(recorder* r) {
        bufferManager.setRecorder(r);
        btree.filename = "_";
        btree.rootPos = -1;
        
    }
    
    int createIndex(std::string filename, int typeId = 0, int strLen = 0) {
        recorder* r;
        r = bufferManager.getRecorder();
        r->createTable(filename, BLOCK_SIZE);
        
        this->filename = filename;
        
        bufferManager.fname = filename;
        
        btree.setBufferManager(&bufferManager);
        btree.setFilename(filename);
        Node rootNode(true, true);
        Block rootBlock = bufferManager.newBlock(filename);
        
        btree.setTypeId(typeId);
        btree.setStrLen(strLen);
        btree.setRootNode(rootNode);
        btree.setRootPos(rootBlock.pos);
        rootNode.write2Blk(rootBlock, btree.getTypeId(), btree.getStrLen(), &bufferManager);
        
        return rootBlock.pos;
    }
    
    int getIndex(std::string filename, int rootPos, int typeId = 0, int strLen = 0){
        recorder* r;
        r = bufferManager.getRecorder();
        
        if( btree.filename == filename && btree.rootPos == rootPos )
            return rootPos;
        
        
        this->filename = filename;
        btree.setBufferManager(&bufferManager);
        btree.setFilename(filename);
        
        Block rootBlock;
        bufferManager.getBlock(filename, rootPos, rootBlock);
        Node rootNode(rootBlock);
        btree.setTypeId(typeId);
        btree.setStrLen(strLen);
        btree.setRootNode(rootNode);
        btree.setRootPos(rootBlock.pos);
        
        return rootBlock.pos;
    }
    
    void insertIntoIndex(std::map<AttrType, int> datalist) {
        std::map<AttrType, int>::iterator it;
        for( it = datalist.begin(); it != datalist.end(); it++ ) {
            std::cout << "==>insertIntoIndex" <<it->first.datai << " " << it->second << std::endl;
            btree.insert(it->first, it->second);
            //system("pause");
        }
    }
    
    std::vector<int> findInIndex(std::vector<AttrType> datalist) {
        std::vector<int> rst;
        
        std::vector<AttrType>::iterator it;
        for( it = datalist.begin(); it != datalist.end(); it++ ) {
            int tmpRst;
            //tmpRst = btree._findLeaf(*it); 
            tmpRst = btree.find(*it);
            //std::cout << (*it).datai << "-->" << tmpRst << std::endl;
            rst.push_back(tmpRst);
        }
        return rst;
    }
    
    void deleteFromIndex(std::vector<AttrType> datalist) {
        std::vector<int> rst;
        
        std::vector<AttrType>::iterator it;
        for( it = datalist.begin(); it != datalist.end(); it++ ) {
            btree.remove(*it);
        }	
    }
    
    void dropIndex(std::string filename) {
        bufferManager.r->dropTable(filename);
    }
    
};

#endif