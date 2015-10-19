// B+ Tree
// Bohan Li

/*
数据类型 int, char(n) 1<=n<=255, float
B+树属性均为单属性、单值

B+树索引的创建、删除
键值的插入删除
等值查询

*/

// B+ Tree

#include <iostream>
#include <cstdlib>
#include <vector>
#include <stack>
#include <iterator>
#include <algorithm>


class AttrType {
public:
	int datai;
	float dataf;
	std::string datas;
	AttrType() {}
	AttrType(int i): datai(i) {}
	AttrType(float f): dataf(f) {}
	AttrType(std::string s): datas(s) {}
};

class Node {
private:
	bool isroot, isleaf;
	std::vector<int> P;
	std::vector<AttrType> K;
	int readi(char *&mem);
	float readf(char *&mem);
	std::string reads(char *&mem, int strLen);
	void writei(int i, char *&mem);
	void writef(float f, char *&mem);
	void writes(std::string s, int strLen ,char *&mem );

public:
	Node();
	Node(bool _isroot, bool _isleaf);
	Node(const Block &blk);
	void write2Blk(Block &blk, int typeId, int strLen);
	bool isRoot();
	bool isLeaf();
	void appendK(const AttrType &k);
	void appendP(int _p);
	void popK();
	void popP();
	int getKSize();
	int getPSize();
	int getK(int i);
	int getP(int i);
	void insert(const AttrType &k, int p, int i);
	void remove(const AttrType &k, int p, int i);
};


class BPlusTree {
private:
	std::string filename; // {tableName}_{indexName}.idx
	Node rootNode;
	int rootP;
	int fanout;
	int typeId, strLen;
	std::stack<int> stk;
	BufferManager bufferManager;
	void setRootNode(Node n);
	void setRootPos(int p);
	int _findLeaf(const AttrType &k);
	void _insertNewBlk(Block &blk1, const AttrType &k, Block &blk2);
	void _remove(const Block &blk, AttrType k);

public:
	BPlusTree();
	BPlusTree(const std::string &_filename);
	~BPlusTree();
	int find(const AttrType &k);
	void insert(const AttrType &k, int p);
	void remove(const AttrType &k);
	std::vector<int> findLeft(const AttrType &k, bool (*cmp)(const AttrType &a,const AttrType &b));
	std::vector<int> findRight(const AttrType &k, bool (*cmp)(const AttrType &a,const AttrType &b));
};
