// B+ Tree

#include "b_plus_tree.h"

int Node::readi(char *&mem) {
	int rst;
	memcpy(&rst, mem, sizeof(int));
	mem += sizeof(int);
	return rst;
}

float Node::readf(char *&mem) {
	float rst;
	memcpy(&rst, mem, sizeof(float));
	mem += sizeof(float);
	return rst;
}

std::string Node::reads(char *&mem, int strLen) {
	static rst[BLOCK_SIZE];
	memcpy(&rst, mem, sizeof(char)*strLen);
	rst[strLen] = 0;
	mem += sizeof(char)*strLen;
	return rst;
}

void Node::writei(int i, char *&mem) {
	memcpy(mem, &i, sizeof(int));
	mem += sizeof(int);
}

void Node::writef(float f, char *&mem) {
	memcpy(mem, f, sizeof(float));
	mem += sizeof(float);
}

void Node::writes(std::string s, int strLen ,char *&mem ) {
	memcpy(mem, s., sizeof(char)*strLen);
	mem += sizeof(char)*strLen;
}

Node::Node() {}

Node::Node(bool _isroot, bool _isleaf) : isroot(_isroot), isleaf(_isleaf) {}

Node::Node(const Block &blk) { // notice here
	static unsigned char mem[BLOCK_SIZE];
	memcpy(mem, blk.data, BLOCK_SIZE);

	isleaf = mem[0]; isRoot = mem[1];
	mem += 2*sizeof(char);
	int pSize = readi(mem), kSize = readi(mem), typeId = readi(mem), strLen = readi(mem);

	for( int i = 0; i < pSize; i++ ) {
		int p = readi(m);
		P.push_back(p);
	}

	for( int i = 0; i < kSize; i++ ) {
		AttrType k;

		if( typeId == 0 ) {
			k = AttrType(readi(mem));
		}
		else if( typeId == 1 ) {
			k = AttrType(readf(mem));
		}
		else if( typeId == 2 ) {
			k = AttrType(reads(mem, strLen));
		}
		K.push_back(k);
	}
}

void Node::write2Blk(Block &blk, int typeId, int strLen) {
	char *mem = blk.data;

	mem[0] = isleaf; mem[1] = isRoot;
	mem += 2*sizeof(char);
	writei(pSize, mem), writei(kSize, mem), writei(typeId, mem), writei(strLen, mem);

	for( int i = 0; i < (int)P.size(); i++ ) {
		writei(P[i], mem);
	}

	for( int i = 0; i < (int)K.size(); i++ ) {
		if( typeId == 0 ) {
			readi(K[i].datai, mem);
		}
		else if( typeId == 1 ) {
			readf(K[i].dataf, mem);
		}
		else if( typeId == 2 ) {
			reads(K[i].datas, strLen, mem);
		}
	}

	BufferManager bufferManager;
	bufferManager.writeBlock(blk);
}

bool Node::isRoot() { return isroot; }
bool Node::isLeaf() { return isleaf; }

void Node::appendK(const AttrType &k) { K.push_back(_k); }
void Node::appendP(int _p) { P.push_back(_p);}
void Node::popK() { K.pop_back(); }
void Node::popP() { P.pop_back(); }

int Node::getKSize() { return K.size(); }
int Node::getPSize() { return P.size(); }
int Node::getK(int i) { return K[i]; }
int Node::getP(int i) { return P[i]; }

void Node::insert(const AttrType &k, int p, int i) {
	K.insert(v.begin()+i, k);
	P.insert(v.begin()+i, p);
}

void Node::remove(const AttrType &k, int p, int i) {
	K.erase(v.begin()+i, k);
	P.erase(v.begin()+i, p);
}

};


void BPlusTree::etRootNode(Node n): rootNode(n) {}

void BPlusTree::setRootPos(int p): rootP(p) {}

int BPlusTree::_findLeaf(const AttrType &k) {
	Node node(bufferManager.getBlock(filename, rootP));
	int p = rootP;
	while( !stk.empty() ) {
		stk.pop();
	}
	stk.push(p);

	while( !node.isLeaf() ) {
		for(int i = 0; i < node.getPSize(); i++ ) {
			if( i == node.getPSize() - 1 || k < node.getK(i) ) {
				p = node.getP(i);
				break;
			}
		}
		node = Node(bufferManager.getBlock(filename, p));
		stk.push(p);
	}

	return p;
}

void BPlusTree::_insertNewBlk(Block &blk1, const AttrType &k, Block &blk2) {
	Node n1 = Node(blk1), n2 = Node(blk2);
	if( n1.isRoot() ) {
		Node newNode(true, false);
		Block newBlk = bufferManager.newBlock();
		newNode.appendP(blk1.getPos())
		newNode.appendK(k);
		newNode.appendP(blk2.getPos());

		// update the former root and write it to block
		n1.setRoot(false);
		n1.write2Blk(blk1, typeId, strLen);

		// update the B+ tree and write new parent to block
		setRootNode(newNode); 
		setRootPos(newBlk.getPos());
		newNode.write2Blk(newBlk, typeId, strLen);
	}
	else {
		// retrieve the parent
		Block opBlk = bufferManager.getBlock(filename, stk.top());
		Node opNode(opBlk);

		for( int i = 0; i < opNode.getPSize(); i++ ) {
			if( opNode.getP(i) == blk1.getPos() ) {
				opNode.insertP(k, blk2.getPos(), i+1);
			}
		}

		if( opNode.getKSize() < fanout ) {
			opNode.write2Blk(opBlk, typeId, strLen);
		}
		else { // this parent node still need to split
			Node npNode1(false, false), npNode2(false, false);
			Block npBlk = bufferManager.newBlock(filename);

			for( int i = 0; i < (fanout + 1) / 2 - 1; i++ ) {
				npNode1.appendK(opNode.getK(i));
				npNode1.appendP(node.getP(i));
			}
			AttrType nk = node.getK(i);
			npNode1.appendP(node.getP(i));
			for( int i = (fanout + 1) / 2; i < (fanout + 1) / 2; i++ ) {
				npNode2.appendK(node.getK(i));
				npNode2.appendP(node.getP(i));
			}
			npNode2.appendP(node.getP(i));

			npNode1.write2Blk(opBlk, typeId, strLen);
			npNode2.write2Blk(npBlk, typeId, strLen);

			_insertNewBlk(opBlk, nk, npBlk);
		}
	}
}

void BPlusTree::_remove(const Block &blk, AttrType k) {
	Node node(blk);

	// delete the corresponding p, k at first
	for( int i = 0; i < node.getKSize(); i++ ) {
		if( node.getK(i) == k ) {
			node.remove(i);
			break;
		}
	}

	// then check whether the size of the node is too small
	bool isRootWithSoleChild =  node.isRoot() && !node.isLeaf() && node.getPSize == 1;

	bool isNotRootAndTooSmall = ( node.isLeaf() && node.KSize() < fanout / 2 ) || ( !node.isLeaf() && node.KSize() < (fanout+1) / 2 )


	if( isRootWithSoleChild ) { // If the root node has only one pointer after deletion, it is deleted and the sole child becomes the root. 
		Block soleChildBlk = bufferManager.getBlock(node.getP(0));
		node soleChildNode(soleChildBlk);
		soleChildNode.setRoot(true);
		setRootNode(soleChildNode);
		setRootPos(soleChildBlk.getPos());
		soleChildNode.write2Blk(soleChildBlk, typeId, strLen);
		bufferManager.deleteBlock(blk);

		return;
	}
	else if( isNotRootAndTooSmall ) {
		stk.pop();
		Block parentBlk = bufferManager.getBlock(stk.pop());
		Node parentNode(parentNode);

		// find sibling
		bool isLeftSibling = true;
		for( int i = 0; i < parentNode.getPSize(); i++ ) {
			if( parentNode.getP(i) == blk.getPos() ) {
				if( i > 0 ) {
					int siblingPos = parentNode.getP(i-1);
					AttrType nk = parentNode.getK(i);
				}
				else {
					int siblingPos = parentNode.getP(i+1);
					AttrType nk = parentNode.getK(i+1);
					isLeftSibling = false;
				}
				Block siblingBlk = bufferManager.getBlock(siblingPos);
				Node siblingNode(siblingBlk);
				break;
			}
		}

		// merge
		if( !isLeftSibling ) {
			std::swap(node, siblingNode);
			std::swap(blk, siblingBlk);
		}

		if( siblingNode.isLeaf() ) {
			siblingNode.popP();
		}
		else {
			siblingNode.appendK(nk);
		}

		for( int i = 0; i < node.getPSize(); i++ ) {
			siblingNode.appendP(node.getP(i));
			if( i < node.getPSize() - 1 ) {
				siblingNode.appendK(node.getK(i));
			}
		}

		if( siblingNode.getPSize() + node.getPSize() < fanout ) { // can be merged
			siblingNode.writeToBlk(siblingBlk);
			bufferManager.deleteBlock(blk);
			_remove(parentBlk, nk);
		}
		else { // split
			Node n1(false, false), n2(false, false);

			for( int i = 0; i < (siblingNode.getPSize() + 1) / 2 - 1; i++ ) {
				n1.appendK(siblingNode.getK(i));
				n1.appendP(siblingNode.getP(i));
			}
				
			n1.appendP(siblingNode.getP(i));
			for( int i = (siblingNode.getPSize() + 1) / 2; i < (siblingNode.getPSize() + 1) / 2; i++ ) {
				n2.appendK(siblingNode.getK(i));
				n2.appendP(siblingNode.getP(i));
			}
			
			n2.appendP(siblingNode.getP(i));

			n1.write2Blk(siblingBlk, typeId, strLen);
			n2.write2Blk(blk, typeId, strLen);

			if( !isLeftSibling ) {
				std::swap(blk, siblingBlk);
			}
		}
	}
	else {
		node.write2Blk(blk, typeId, strLen);
	}
}

BPlusTree::BPlusTree() {}
BPlusTree::BPlusTree(const std::string &_filename): filename(_filename) {
//
typeId = 0;
strLen = 0;
}

BPlusTree::~BPlusTree() {}


int BPlusTree::find(const AttrType &k) {
	int blkPos = _findLeaf(k);
	Node node(bufferManager.getBlock(filename, blkPos));
	for(int i = 0; i < node.getKSize(); i++ ) {
		if( node.getK(i) == k ) {
			return getP(i);
		}
	}

	return -1;
}

void BPlusTree::insert(const AttrType &k, int p) {
	int blkPos = _findLeaf(k);
	Block oldBlk = bufferManager.getBlock(filename, blkPos);
	Node node(oldBlk);

	// first insert k,p into the corresponding leaf node
	for(int i = 0; i < node.getPSize(); i++ ) {
		if( i == node.getPSize() - 1 || k < node.getK(i) ) {
			node.insert(k, p, i);
			break;
		}
	}

	if( node.getKSize() < fanout ) {
		node.write2Blk(oldBlk, typeId, strLen);
	}
	else { // need to split
		Node n1(false, true), n2(false, true);
		Block newBlk = bufferManager.newBlock(filename);

		for( int i = 0; i < (fanout + 1) / 2; i++ ) {
			n1.appendK(node.getK(i));
			n1.appendP(node.getP(i));
		}
		n1.appendP(n2);
		for( int i = (fanout + 1) / 2; i < (fanout + 1) / 2; i++ ) {
			n2.appendK(node.getK(i));
			n2.appendP(node.getP(i));
		}
		n2.appendP(node.getP(i));

		n1.write2Blk(oldBlk, typeId, strLen);
		n2.write2Blk(newBlk, typeId, strLen);

		_removeinsertNewBlk(oldBlk, n2.getK(0), newBlk);
	}
}

void BPlusTree::remove(const AttrType &k) {
	int blkPos = _findLeaf(k);
	Block blk = bufferManager.getBlock(filename, blkPos);
	Node node(blk);
	_remove(blk, k);
}


std::vector<int> BPlusTree::findLeft(const AttrType &k, bool (*cmp)(const AttrType &a,const AttrType &b)) {
	std::vector<int> rst;

	int pos = rootPos;
	Node node(bufferManager.getBlock(filename, pos));
	while( !node.isLeaf() ) {
		pos = node.getP(0);
		Node node(bufferManager.getBlock(filename, pos));
	}

	while( ~pos ) {
	// attention when the last p of the last leaf is null
		for( int i = 0; i < node.getKSize(); i++ ) {
			if( cmp(node.getK(i), k) ) {
				rst.push_back(node.getP(i));
			}
			else {
				return rst;
			}
		}
		pos = node.getP(node.PSize() - 1);
	}
}

std::vector<int> BPlusTree::findRight(const AttrType &k, bool (*cmp)(const AttrType &a,const AttrType &b)) {
	std::vector<int> rst;

	int pos = _findLeaf(k);
	while( ~pos ) {
		Node node(bufferManager.getBlock(filename, pos));
		for( int i = 0; i < node.getKSize(); i++ ) {
			if( cmp(node.getK(i), k) ) {
				rst.push_back(node.getP(i));
			}
			else {
				return rst;
			}
		}
		pos = node.getP(node.PSize() - 1);
	}
}

