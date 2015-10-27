#ifndef _recorder_
#define _recorder_
#include <fstream>
#include <vector>
#include <stdio.h>
#include <map>
#include "bufferManager.hpp"

bool isAllZero(char * p, int num)
{
    char * sp = (char*)p;
    for(int i = 0; i < num; i++)
    {
        if(*(sp+i) != 0)
            return false;
    }
    return true;
}

bufferManager * bm = new bufferManager();
using namespace std;
class freeList
{
private:
	vector<long> offset;
	string fileAddr;
	short elementSize;
	int numOfblocks;
public:
	freeList(string& fileAddr, short elementSize);
	~freeList();
	short getElementSize();
	void setIndex();
	void addElement(long offset);
    bool isEmpty();
    long getNext();
	friend class recorder;
};

long freeList::getNext()
{
    long offset = this->offset[0];
    this->offset.erase(this->offset.begin());
    return offset;
}

bool freeList::isEmpty()
{
    return this->offset.size() == 0;
}

void freeList::addElement(long offset)
{
	this->offset.push_back(offset);
}

short freeList::getElementSize()
{
	return elementSize;
}

void freeList::setIndex()
{
	int k = 0;
	while(1)
	{
		if(elementSize*(k+1) < 4096)
			offset.push_back(numOfblocks*4096 + elementSize*k);
		else
			break;
		k++;
	}
	numOfblocks += 1;
}

freeList::freeList(string& fileAddr, short elementSize)
{
	fstream file;
	this->elementSize = elementSize;
	int size = 0;
	this->fileAddr = fileAddr;
	file.open(fileAddr);
	file >> numOfblocks;
	file >> size;
	for(int i = 0; i < size; i++)
	{
		long temp;
		file >> temp;
		offset.push_back(temp);
	}
	file.close();
}

freeList::~freeList()
{
	fstream file;
	file.open(fileAddr, ofstream::out| ios_base::trunc);
	file << numOfblocks << " ";
	file << offset.size() << " ";
	vector<long>::iterator p;
	for(p = offset.begin(); p != offset.end(); p++)
		file << *p << " ";
	file.close();
}

class conditionJudge
{
    
};



class recorder
{
private:
	map<string, int> table2Freelist;
	map<int, string> freelist2Table;
	vector<freeList* > freeListVector;
	string dataPath;
public:
	recorder(string& fileAddr);//finish
	~recorder();//finish
	bool createTable(string& tableName, short elementSize);//finish

	bool deleteNoWhere(string& tableName);//fiish
	
	bool deleteWhereNoIndex(string& tableName, conditionJudge& judger);
	bool deleteWhereIndex(string& tableName, long offset, conditionJudge& judger);

	long insert(void* data, string& tableName);

	vector<void*> selectNoWhere(string& tableName);
	vector<void*> selectWhereIndex(string& tableName, vector<long> offset, conditionJudge& judger);
	vector<void*> selectWhereNoIndex(string& tableName, conditionJudge& judger);
};

vector<void*> recorder::selectWhereNoIndex(string& tableName, conditionJudge& judger)
{
    int index = table2Freelist[tableName];
    vector<void*> vec;
    freeList* fp = freeListVector[index];
    int eachSize = fp->getElementSize();
    int numBlocks = fp->numOfblocks;
    for(int i = 0; i < numBlocks; i++)
    {
        int idx = bm->getIndex(tableName, i*BUFFERSIZE_LIMIT);
        if(idx == -1)
        {
            idx = bm->mallocBuffer();
            bm->readData(tableName, i*BUFFERSIZE_LIMIT, idx);
        }
        for(int k = 0; (k+1)*eachSize < BUFFERSIZE_LIMIT; k++)
        {
            char * p = (char *)(bm->retrieveData(idx, i*BUFFERSIZE_LIMIT + k*eachSize));
            if(judger.isSatisfied(p) == true)
            {
                vec.push_back(p);
            }
        }
    }
    return vec;
}

vector<void*> recorder::selectWhereIndex(string& tableName, vector<long> offset, conditionJudge& judger)
{
    int index = table2Freelist[tableName];
    vector<void*> vec;
    freeList* fp = freeListVector[index];
    int eachSize = fp->getElementSize();
    int numBlocks = fp->numOfblocks;
    vector<long>::iterator p;
    for(p = offset.begin(); p != offset.end(); p++)
    {
        int idx = bm->getIndex(tableName, *p);
        if(idx == -1)
        {
            idx = bm->mallocBuffer();
            bm->readData(tableName, *p, idx);
        }
        long pos = *p - *p/4096*4096;
        char * p = (char *)(bm->retrieveData(idx, *p));
        if(judger.isSatisfied(p) == true)
        {
            vec.push_back(p);
        }
    }
    return vec;
}

vector<void*> recorder::selectNoWhere(string& tableName)
{
    int index = table2Freelist[tableName];
    vector<void*> vec;
    freeList* fp = freeListVector[index];
    int eachSize = fp->getElementSize();
    int numBlocks = fp->numOfblocks;
    for(int i = 0; i < numBlocks; i++)
    {
        int idx = bm->getIndex(tableName, i*BUFFERSIZE_LIMIT);
        if(idx == -1)
        {
            idx = bm->mallocBuffer();
            bm->readData(tableName, i*BUFFERSIZE_LIMIT, idx);
        }
        for(int k = 0; (k+1)*eachSize < BUFFERSIZE_LIMIT; k++)
        {
            char * p = (char *)(bm->retrieveData(idx, i*BUFFERSIZE_LIMIT + k*eachSize));
            if(isAllZero(p, eachSize) == false)
            {
                vec.push_back(p);
            }
        }
    }
    return vec;
}


long recorder::insert(void* data, string& tableName)
{
    int index = table2Freelist[tableName];
    freeList* fp = freeListVector[index];
    long offset;
    if(fp->isEmpty() == true)
        fp->setIndex();
    offset = fp->getNext();
    index = bm->getIndex(tableName, offset);
    if(index == -1)
    {
        index = bm->mallocBuffer();
        bm->readData(tableName, offset, index);
    }
    bm->storeData(index, data, offset, fp->elementSize);
    return offset;
}


//全局偏移
bool recorder::deleteWhereIndex(string& tableName, long offset, conditionJudge& judger)
{
    int num, eachSize;
    char ch[4096] = {0};
    void * p;
    freeList* fp = freeListVector[table2Freelist[tableName]];
    num = fp->numOfblocks;
    eachSize = fp->elementSize;
    //int index = bm->mallocBuffer();
    int index = bm->getIndex(tableName, offset);
    if(index == -1)
    {
        index = bm->mallocBuffer();
        bm->readData(tableName, offset, index);
    }
    p = bm->retrieveData(index, offset);
    if(judger.isSatisfied(p) == true)
    {
        bm->storeData(index, ch, offset, eachSize);
        fp->addElement(offset);
    }
   
    return true;
}


bool recorder::deleteWhereNoIndex(string& tableName, conditionJudge& judger)
{
    int num, eachSize;
    char ch[4096] = {0};
    void * p;
    freeList* fp = freeListVector[table2Freelist[tableName]];
    num = fp->numOfblocks;
    eachSize = fp->elementSize;
    for(int i = 0; i < num; i++)
    {
        int index = bm->mallocBuffer();
        bm->readData(tableName, i*BUFFERSIZE_LIMIT, index);
        for(int j = 0; (j+1)*eachSize < 4096; j++)
        {
            p = bm->retrieveData(index, j*eachSize + i*BUFFERSIZE_LIMIT);
            if(judger.isSatisfied(p) == true)
            {
                bm->storeData(index, ch, j*eachSize + i*BUFFERSIZE_LIMIT, eachSize);
                fp->addElement(j*eachSize);
            }
        }
    }
    return true;
}


bool recorder::deleteNoWhere(string& tableName)
{
	freeList* fl;
	fl = freeListVector[table2Freelist[tableName]];
	for(int i = 0; i < fl->numOfblocks; i++)
	{
		long index = bm->getIndex(tableName, i*BUFFERSIZE_LIMIT);
		if(index != -1)
		{
			bm->freeBuffer(index, false);
		}
	}
	string filead;
	filead = tableName + "freeList.data";
	remove(filead.c_str());
	remove(tableName.c_str());
	int index = table2Freelist[tableName];
	table2Freelist.erase(tableName);
	freelist2Table.erase(index);
	freeListVector.erase(freeListVector.begin() + index);
	createTable(tableName, fl->elementSize);
	fl->offset.clear();
	fl->numOfblocks = 1;
	delete fl;
	return true;
}



bool recorder::createTable(string& tableName, short elementSize)
{
	string filePath = tableName + "freeList.data";
	fstream newfile;
	newfile.open(filePath, ofstream::out);
	newfile << 1 << " " << 0 << " ";
	newfile.close();
	freeList *fl = new freeList(filePath, elementSize);
	char buff = 0;
	table2Freelist.insert(pair<string,int>(tableName, freeListVector.size()));
	freelist2Table.insert(pair<int, string>(freeListVector.size(), tableName));
	freeListVector.push_back(fl);
	fl->setIndex();
	fstream file;
	file.open(tableName, ofstream::out);
	for(int i = 0; i < 4096; i++)
		file << buff;
	file.close();
	return true;
}


/*
*读入每个表的freeList，并创建map， 表名<->freelist下标
*/

recorder::recorder(string& fileAddr)
{
	fstream file;
	file.open(fileAddr);
	dataPath =fileAddr;
	int size = 0;
	file >> size;
	string tableName, dataPath;
	int k = 0;
	short elementSize;
	while(k < size)
	{
		file >> tableName >> dataPath >> elementSize;
		freeListVector.push_back(new freeList(dataPath, elementSize));
		table2Freelist.insert(pair<string, int>(tableName, k));
		freelist2Table.insert(pair<int, string>(k, tableName));
		k++;
	}
	file.close();
}

/*
*写回文件，包括纪录每个表的文件中空余空间的位置。
*以及目前有的表名、表地址、表纪录大小
*/

recorder::~recorder()
{
	fstream	file;
	file.open(dataPath, ofstream::out| ios_base::trunc);
	int k = freeListVector.size();
	file << k << " ";
	for(int i = 0; i < k; i++)
	{
		file << freelist2Table[i] << " " << (*freeListVector[i]).fileAddr << " " << (*freeListVector[i]).elementSize << " ";
	}
	for(int i = 0; i < k; i++)
	{
		delete freeListVector[i];
	}
	file.close();
}



#endif
