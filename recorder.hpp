#ifndef _recorder_
#define _recorder_
#include <fstream>
#include <vector>
#include <stdio.h>
#include <map>
#include "bufferManager.hpp"
#include "auxiliary.hpp"

using namespace std;

template<typename T> bool isEqual(void* t1, string& t2, int length)
{
    T * val = (T*) t1;
    
}





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


/*
 class WhereForRecorder
 {
 public:
 int type;  0-int 1-float 2-char
 int length
 string op;
 string value;
 int offset; //the first: 0
 };
 */



class conditionJudge
{
private:
    vector<WhereForRecorder>* cond;
    int elementSize;
public:
    conditionJudge(vector<WhereForRecorder>* cond, int elementSize):cond(cond), elementSize(elementSize){};
    bool isSatisfied(char *p);
    bool isSatisfied(void *pt);
    ~conditionJudge(){delete cond;}
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
    bool updateWhere(string& tableName, long offset, void * data);
    bool deleteWhereNoIndex(string& tableName, conditionJudge& judger);
    bool deleteWhereIndex(string& tableName, long offset, conditionJudge& judger);
    
    long insert(void* data, string& tableName);
    
    vector<void*> selectNoWhere(string& tableName);
    vector<void*> selectWhereIndex(string& tableName, vector<long> offset, conditionJudge& judger);
    vector<void*> selectWhereNoIndex(string& tableName, conditionJudge& judger);
};

#endif
