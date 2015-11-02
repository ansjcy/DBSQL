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
	static char rst[BLOCK_SIZE];
	memcpy(&rst, mem, sizeof(rst[0])*strLen);
	rst[strLen] = 0;
	mem += sizeof(rst[0])*strLen;
	return rst;
}

void Node::writei(int i, char *&mem) {
	memcpy(mem, &i, sizeof(i));
	mem += sizeof(i);
}

void Node::writef(float f, char *&mem) {
	memcpy(mem, &f, sizeof(f));
	mem += sizeof(f);
}

void Node::writes(std::string s, int strLen, char *&mem ) {
	memcpy(mem, s.c_str(), strLen);
	mem += sizeof(s.c_str()[0])*strLen;
}

Node::Node() {}

Node::Node(bool _isroot, bool _isleaf) : isroot(_isroot), isleaf(_isleaf) {}

void Node::resetByBlock(const Block &blk) {
	char* mem = (char*)malloc(BLOCK_SIZE);
	memcpy(mem, blk.data, BLOCK_SIZE);

	isleaf = mem[0]; isroot = mem[1];

	mem += 2*sizeof(char);
	int pSize = readi(mem), kSize = readi(mem), typeId = readi(mem), strLen = readi(mem);

	//system("pause");

	std::vector<AttrType>::iterator iter_K=K.begin();
	for ( ;iter_K!=K.end();) iter_K=K.erase(iter_K);

	std::vector<int>::iterator iter_P=P.begin();
	for ( ;iter_P!=P.end();) iter_P=P.erase(iter_P);

	int p = 0;

	for( int i = 0; i < pSize; i++ ) {
		p = readi(mem);
		P.push_back(p);
	}


	for( int i = 0; i < kSize; i++ ) {
		AttrType k;

		if( typeId == 0 ) { // int
			k = AttrType(readi(mem)); 
		}
		else if( typeId == 1 ) { // float
			k = AttrType(readf(mem));
		}
		else if( typeId == 2 ) { // string
			k = AttrType(reads(mem, strLen));
		}

		K.push_back(k);
	}	
}

Node::Node(const Block &blk) { // notice here
	char* mem = (char*)malloc(BLOCK_SIZE);
	memcpy(mem, blk.data, BLOCK_SIZE);

	isleaf = mem[0]; isroot = mem[1];

	mem += 2*sizeof(char);
	int pSize = readi(mem), kSize = readi(mem), typeId = readi(mem), strLen = readi(mem);

	//system("pause");

	int p = 0;

	for( int i = 0; i < pSize; i++ ) {
		p = readi(mem);
		P.push_back(p);
	}


	for( int i = 0; i < kSize; i++ ) {
		AttrType k;

		if( typeId == 0 ) { // int
			k = AttrType(readi(mem)); 
		}
		else if( typeId == 1 ) { // float
			k = AttrType(readf(mem));
		}
		else if( typeId == 2 ) { // string
			k = AttrType(reads(mem, strLen));
		}

		K.push_back(k);
	}
}



void Node::write2Blk(Block &blk, int typeId, int strLen, BufferManager* bufferManager) {
	char* mem = blk.data;

	mem[0] = isleaf; mem[1] = isroot;
	mem += 2*sizeof(char);
	writei(getPSize(), mem), writei(getKSize(), mem), writei(typeId, mem), writei(strLen, mem);

	std::cout << "getPSize()" << getPSize() <<std::endl;
	std::cout << "getKSize()" << getKSize() <<std::endl;

	for( int i = 0; i < getPSize(); i++ ) {
		std::cout << "P[i]" << P[i] << std::endl;
		writei(P[i], mem);
	}

	for( int i = 0; i < getKSize(); i++ ) {
		std::cout << "K[i]" << K[i].datai << std::endl;
		if( typeId == 0 ) {
			writei(K[i].datai, mem);
		}
		else if( typeId == 1 ) {
			writef(K[i].dataf, mem);
		}
		else if( typeId == 2 ) {
			writes(K[i].datas, strLen, mem);
		}
	}

	std::cout << "Node::write2Blk " << blk.pos/BLOCK_SIZE << std::endl;

	//BufferManager bufferManager;
	bufferManager->writeBlock(blk);
}



bool Node::isRoot() { return isroot; }
bool Node::isLeaf() { return isleaf; }

void Node::appendK(const AttrType &_k) { K.push_back(_k); }
void Node::appendP(int _p) { P.push_back(_p);}
void Node::popK() { K.pop_back(); }
void Node::popP() { P.pop_back(); }

int Node::getKSize() { return K.size(); }
int Node::getPSize() { return P.size(); }

AttrType Node::getK(int i) { return K[i]; }
int Node::getP(int i) { return P[i]; }

void Node::insert(const AttrType &k, int p, int i) {
	
	if( i < K.size() - 1 ) {
		K.insert(K.begin()+i, k);
		P.insert(P.begin()+i, p);
	} else {
		//std::cout << "k=" << k.datai << " p=" << p << std::endl;
		K.push_back(k);
		P.insert(P.begin()+i, p);
	}
}

void Node::remove(const AttrType &k, int p, int i) {
	K.erase(K.begin()+i);
	P.erase(P.begin()+i);
}


void BPlusTree::setRootNode(Node n) { rootNode = n; }

void BPlusTree::setRootPos(int p) { rootPos = p; }

int BPlusTree::_findLeaf(const AttrType &k) {

	Block tmpBlock_;
	bufferManager->getBlock(filename, rootPos, tmpBlock_);
	Node node(tmpBlock_);

	//Node node(true, true);
	int p = rootPos;
	while( !stk.empty() ) {
		stk.pop();
	}

	stk.push(p);

	bool testBool = node.isLeaf();

	while( !testBool ) {
		testBool = node.isLeaf();
		std::cout << "node.isLeaf()" << testBool << std::endl;
		for(int i = 0; i < node.getPSize(); i++ ) {
			if( i == node.getPSize() - 1 || k < node.getK(i) ) {
				p = node.getP(i);
				break;
			}
		}



		Block tmpBlock_2;
		bufferManager->getBlock(filename, p, tmpBlock_2);
		node.resetByBlock(tmpBlock_2);
		
		stk.push(p);
		testBool = node.isLeaf();
		std::cout << "node.isLeaf()" << testBool << std::endl;
		//system("pause");
	}

	std::cout << "_findLeaf return []" << p << std::endl;	

	return p;
}


void BPlusTree::_insertNewBlk(Block &blk1, const AttrType &k, Block &blk2) {
	Node n1 = Node(blk1), n2 = Node(blk2);
	int i;
	if( n1.isRoot() ) {
		Node newNode(true, false);
		Block newBlk = bufferManager->newBlock(filename);
		newNode.appendP(blk1.getPos());
		newNode.appendK(k);
		newNode.appendP(blk2.getPos());

		// update the former root and write it to block
		n1.setRoot(false);
		n1.write2Blk(blk1, typeId, strLen, bufferManager);

		n2.setRoot(false);

		std::cout << "_insertNewBlk == n1.isLeaf() " << (int)n1.isLeaf() << std::endl;
		std::cout << "_insertNewBlk == n2.isLeaf() " << (int)n2.isLeaf() << std::endl;

		n2.write2Blk(blk2, typeId, strLen, bufferManager);

		// update the B+ tree and write new parent to block
		setRootNode(newNode); 
		setRootPos(newBlk.getPos());
		newNode.write2Blk(newBlk, typeId, strLen, bufferManager);
	}
	else {
		// retrieve the parent
		stk.pop();
		Block opBlk;
		bufferManager->getBlock(filename, stk.top(), opBlk);
		Node opNode(opBlk);

		for( i = 0; i < opNode.getPSize(); i++ ) {
			if( opNode.getP(i) == blk1.getPos() ) {
				opNode.insert(k, blk2.getPos(), i+1);
			}
		}

		if( opNode.getKSize() < fanout ) {
			opNode.write2Blk(opBlk, typeId, strLen, bufferManager);
		}
		else { // this parent node still need to split
			Node npNode1(false, true), npNode2(false, true);
			Block npBlk = bufferManager->newBlock(filename);

			npNode1.setRoot(opNode.isRoot());
			npNode2.setRoot(opNode.isRoot());

			npNode1.setLeaf(opNode.isLeaf());
			npNode2.setLeaf(opNode.isLeaf());

			for( i = 0; i < (fanout + 1) / 2 - 1; i++ ) {
				npNode1.appendK(opNode.getK(i));
				npNode1.appendP(opNode.getP(i));
			}
			AttrType nk = opNode.getK(i);
			npNode1.appendP(opNode.getP(i));
			for( i = (fanout + 1) / 2; i < fanout; i++ ) {
				npNode2.appendK(opNode.getK(i));
				npNode2.appendP(opNode.getP(i));
			}
			npNode2.appendP(opNode.getP(i));

			npNode1.write2Blk(opBlk, typeId, strLen, bufferManager);
			npNode2.write2Blk(npBlk, typeId, strLen, bufferManager);

			_insertNewBlk(opBlk, nk, npBlk);
		}
	}
}

void BPlusTree::_remove(Block blk, AttrType k) {
	Node node(blk);
	int i;

	std::cout << "BPlusTree::_remove == k " << k.datai << std::endl;
	std::cout << "BPlusTree::_remove == blk.pos " << blk.pos << std::endl;

	// delete the corresponding p, k at first
	for( i = 0; i < node.getKSize(); i++ ) {
		if( node.getK(i) == k ) {
			node.remove(k, node.getP(i), i);
			break;
		}
	}

	// then check whether the size of the node is too small
	bool isRootWithSoleChild =  node.isRoot() && !node.isLeaf() && node.getPSize() == 1;

	bool isNotRootAndTooSmall = ( node.isLeaf() && node.getKSize() < fanout / 2 ) || ( !node.isLeaf() && node.getKSize() < (fanout+1) / 2 );

	isNotRootAndTooSmall = isNotRootAndTooSmall && node.isRoot();


	if( isRootWithSoleChild ) { // If the root node has only one pointer after deletion, it is deleted and the sole child becomes the root. 
		Block soleChildBlk;
		bufferManager->getBlock(filename, node.getP(0), soleChildBlk);
		Node soleChildNode(soleChildBlk);
		soleChildNode.setRoot(true);
		setRootNode(soleChildNode);
		setRootPos(soleChildBlk.getPos());
		soleChildNode.write2Blk(soleChildBlk, typeId, strLen, bufferManager);
		bufferManager->deleteBlock(blk);

		return;
	}
	else if( isNotRootAndTooSmall ) {
		Block parentBlk;
		Block siblingBlk;
		Node siblingNode;
		AttrType nk;

		std::cout << "isNotRootAndTooSmall" << std::endl;

		std::cout << "stk.top() = " << stk.top() << std::endl;

		stk.pop();
		
		std::cout << "parentBlk pos = " << stk.size() << std::endl;

		bufferManager->getBlock(filename, stk.top(), parentBlk);
		//stk.pop();
		Node parentNode(parentBlk);

		std::cout << "fuck" << std::endl;

		// find sibling
		bool isLeftSibling = true;
		int siblingPos;

		for( i = 0; i < parentNode.getPSize(); i++ ) {
			if( parentNode.getP(i) == blk.getPos() ) {
				if( i > 0 ) {
					siblingPos = parentNode.getP(i-1);
					nk = parentNode.getK(i);
				}
				else {
					siblingPos = parentNode.getP(i+1);
					nk = parentNode.getK(i);
					isLeftSibling = false;
				}

				std::cout << "siblingPos " << siblingPos/BLOCK_SIZE << std::endl;

				bufferManager->getBlock(filename, siblingPos, siblingBlk);
				siblingNode.resetByBlock(siblingBlk);
				break;
			}
		}

		std::cout << "success up to now" << std::endl;

		

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

		for( i = 0; i < node.getPSize(); i++ ) {
			siblingNode.appendP(node.getP(i));
			if( i < node.getPSize() - 1 ) {
				siblingNode.appendK(node.getK(i));
			}
		}

		for( i = 0; i < siblingNode.getPSize(); i++ ) {
			std::cout << "siblingNode.getP[" << i << "] = " << siblingNode.getP(i) << std::endl;
		}

		for( i = 0; i < siblingNode.getKSize(); i++ ) {
			std::cout << "siblingNode.getK[" << i << "] = " << siblingNode.getK(i).datai << std::endl;
		}

		if( siblingNode.getPSize() <= fanout ) { // can be merged
			std::cout << "siblingNode.write2Blk -- siblingBlk.pos[] " << siblingBlk.pos << std::endl;



			// siblingNode.write2Blk(siblingBlk, typeId, strLen, bufferManager);
			// bufferManager->deleteBlock(blk);
			siblingNode.write2Blk(blk, typeId, strLen, bufferManager);
			bufferManager->deleteBlock(siblingBlk);


			_remove(parentBlk, nk);
		}
		else { // split
			Node n1(false, false), n2(false, false);

			for( i = 0; i < (siblingNode.getPSize() + 1) / 2 - 1; i++ ) {
				n1.appendK(siblingNode.getK(i));
				n1.appendP(siblingNode.getP(i));
			}
				
			n1.appendP(siblingNode.getP(i));
			for( i = (siblingNode.getPSize() + 1) / 2; i < (siblingNode.getPSize() + 1) / 2; i++ ) {
				n2.appendK(siblingNode.getK(i));
				n2.appendP(siblingNode.getP(i));
			}
			
			n2.appendP(siblingNode.getP(i));

			n1.write2Blk(siblingBlk, typeId, strLen, bufferManager);
			n2.write2Blk(blk, typeId, strLen, bufferManager);

			if( !isLeftSibling ) {
				std::swap(blk, siblingBlk);
			}
		}
	}
	else {
		node.write2Blk(blk, typeId, strLen, bufferManager);
	}
}

BPlusTree::BPlusTree() {
	fanout = TEMP_FANOUT;
}

int BPlusTree::find(const AttrType &k) {
	int blkPos = _findLeaf(k);
	Block tmpBlock_;
	bufferManager->getBlock(filename, blkPos, tmpBlock_);
	Node node(tmpBlock_);
	
	
	for(int i = 0; i < node.getKSize(); i++ ) {
		if( node.getK(i) == k ) {
			return node.getP(i);
		}
	}

	return -1;
}

void BPlusTree::insert(const AttrType &k, int p) {
	int i;
	int blkPos = _findLeaf(k);
	
	std::cout << "insert blkPos" << blkPos/BLOCK_SIZE << std::endl;
	//system("pause");

	Block oldBlk;
	bufferManager->getBlock(filename, blkPos, oldBlk);
	Node node(oldBlk); 

	// first insert k,p into the corresponding leaf node

	if( !node.getKSize() ) {
		node.appendK(k);
		node.appendP(p);
		node.appendP(0); // has problem here
	} else {
		for( i = 0; i < node.getKSize(); i++ ) {

			if( k < node.getK(i) ) {
				node.insert(k, p, i);
				break;
			}
		}
		if( i == node.getKSize() ) {
				//std::cout << "k " << k.datai << std::endl;
				//std::cout << "node.getK(i) " << node.getK(i-1).datai << std::endl;
				node.insert(k, p, i);
		}		
	}

	
	if( node.getKSize() < fanout ) {
		std::cout << " node.getKSize() < fanout " << std::endl;
		node.write2Blk(oldBlk, typeId, strLen, bufferManager);
	}
	else { // need to split
		Node n1(false, true), n2(false, true);
		Block newBlk = bufferManager->newBlock(filename);
		AttrType mid;

		n1.setRoot(node.isRoot());
		n2.setRoot(node.isRoot());

		n1.setLeaf(node.isLeaf());
		n2.setLeaf(node.isLeaf());

		for( i = 0; i < (fanout + 1) / 2; i++ ) {
			n1.appendK(node.getK(i));
			n1.appendP(node.getP(i));
		}
		n1.appendP(newBlk.pos); // here is problem!
		mid = node.getK(i);
		for( i = (fanout + 1) / 2; i < fanout; i++ ) {
			n2.appendK(node.getK(i));
			n2.appendP(node.getP(i));
		}
		n2.appendP(node.getP(i));

		n1.write2Blk(oldBlk, typeId, strLen, bufferManager);
		n2.write2Blk(newBlk, typeId, strLen, bufferManager);

		std::cout << "oldBlk " << oldBlk.pos/BLOCK_SIZE << std::endl;
		std::cout << "newBlk " << newBlk.pos/BLOCK_SIZE << std::endl;
		std::cout << "middata " << mid.datai << std::endl;

		_insertNewBlk(oldBlk, mid, newBlk);
	}
}

void BPlusTree::remove(const AttrType &k) {
	int blkPos = _findLeaf(k);

	std::cout << "BPlusTree::remove == k " <<  k.datai << std::endl;
	std::cout << "BPlusTree::remove == blkPos " <<  blkPos/BLOCK_SIZE << std::endl;

	Block blk;
	bufferManager->getBlock(filename, blkPos, blk);
	Node node(blk);
	_remove(blk, k);

	std::cout << "DONE! BPlusTree::remove == k " <<  k.datai << std::endl;
	// system("pause");
}


std::vector<int> BPlusTree::findLeft(const AttrType &k, bool (*cmp)(const AttrType &a,const AttrType &b)) {
	std::vector<int> rst;

	int pos = rootPos;
	Block tmpBlock_;
	bufferManager->getBlock(filename, pos, tmpBlock_);
	Node node(tmpBlock_);
	while( !node.isLeaf() ) {
		pos = node.getP(0);
		bufferManager->getBlock(filename, pos, tmpBlock_);
		Node node(tmpBlock_);
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
		pos = node.getP(node.getPSize() - 1);
	}

	return rst;
}

std::vector<int> BPlusTree::findRight(const AttrType &k, bool (*cmp)(const AttrType &a,const AttrType &b)) {
	std::vector<int> rst;

	int pos = _findLeaf(k);
	while( ~pos ) {
		Block tmpBlock_;
		bufferManager->getBlock(filename, pos, tmpBlock_);
		Node node(tmpBlock_);
		for( int i = 0; i < node.getKSize(); i++ ) {
			if( cmp(node.getK(i), k) ) {
				rst.push_back(node.getP(i));
			}
			else {
				return rst;
			}
		}
		pos = node.getP(node.getPSize() - 1);
	}

	return rst;
}

