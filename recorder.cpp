#include "bufferManager.hpp"
#include "recorder.hpp"
#include <math.h>
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

int min(int a, int b)
{
    if(a < b)
        return a;
    return b;
}

bufferManager * bm = new bufferManager();
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
        if(elementSize*(k+1) <= 4096)
            offset.push_back((numOfblocks-1)*4096 + elementSize*k);
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
    if(numOfblocks != -1)
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
}

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
            //cout << "selectWhereNoIndex: idx == -1 \n";
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

vector<void*> recorder::selectWhereIndex(string& tableName, vector<int> offset, conditionJudge& judger)
{
    int index = table2Freelist[tableName];
    vector<void*> vec;
    freeList* fp = freeListVector[index];
    int numBlocks = fp->numOfblocks;
    vector<int>::iterator p;
    for(p = offset.begin(); p != offset.end(); p++)
    {
        int idx = bm->getIndex(tableName, *p);
        if(idx == -1)
        {
            //cout << "WHere NoIndex: \n";
            idx = bm->mallocBuffer();
            bm->readData(tableName, *p, idx);
        }
        long pos = *p - *p/4096*4096;
        void * dp = (bm->retrieveData(idx, *p));
        if(judger.isSatisfied(dp) == true)
        {
            vec.push_back(dp);
        }
    }
    return vec;
}

bool recorder::updateWhere(string& tableName, long offset, void* data)
{
    fstream file;
    int index = bm->getIndex(tableName, offset);
    
    
    if(index == -1)
    {
        file.open(tableName, ofstream::out);
        file.seekg(offset, ios::beg);
        file.write((char*)data, BUFFERSIZE_LIMIT);
        file.close();
        return true;
    }
    else
    {
        bm->storeData(index, data, offset, BUFFERSIZE_LIMIT);
        return true;
    }
}

vector<void*> recorder::selectNoWhere(string& tableName, vector<int>* addr)
{
    
    int index = table2Freelist[tableName];
    vector<void*> vec;
    freeList* fp = freeListVector[index];
    int eachSize = fp->getElementSize();
    int numBlocks = fp->numOfblocks;
    //cout << "In select No Where\n";
    for(int i = 0; i < numBlocks; i++)
    {
        int idx = bm->getIndex(tableName, i*BUFFERSIZE_LIMIT);
        if(idx == -1)
        {
            idx = bm->mallocBuffer();
            //cout << "SelectNoWhere: idx == -1 \n";
            bm->readData(tableName, i*BUFFERSIZE_LIMIT, idx);
        }
        for(int k = 0; (k+1)*eachSize < BUFFERSIZE_LIMIT; k++)
        {
            char * p = (char *)(bm->retrieveData(idx, i*BUFFERSIZE_LIMIT + k*eachSize));
            if(isAllZero(p, eachSize) == false)
            {
                vec.push_back(p);
                if(addr != NULL)
                    addr->push_back(i*BUFFERSIZE_LIMIT + k*eachSize);
            }
        }
    }
    return vec;
}

bool recorder::dropTable(string& tableName)
{
    int index = table2Freelist[tableName];
    freeList* flp = freeListVector[index];
    remove(flp->fileAddr.c_str());
    for(int i = 0; i < flp->numOfblocks; i++)
    {
        long index = bm->getIndex(tableName, i*BUFFERSIZE_LIMIT);
        if(index != -1)
        {
            bm->freeBuffer(index, false);
        }
    }
    flp->numOfblocks = -1;
    
    table2Freelist.erase(tableName);
    freelist2Table.erase(index);
    vector<freeList *>::iterator fp = freeListVector.begin();
    for(;fp != freeListVector.end(); fp++)
    {
        if(*fp == flp)
        {
            freeListVector.erase(fp);
            break;
        }
    }
    remove(tableName.c_str());
    delete flp;
    return true;
}




long recorder::insert(void* data, string& tableName)
{
    //cout << "in Insert function \n";
    int index = table2Freelist[tableName];
    freeList* fp = freeListVector[index];
    long offset;
    
    if(fp->isEmpty() == true)
        fp->setIndex();
    //cout << "step 2\n";
    offset = fp->getNext();
    cout << "Get offset: " << offset << endl;
    index = bm->getIndex(tableName, offset);
    if(index == -1)
    {
        index = bm->mallocBuffer();
        //cout << "Malloc a new Buffer at " << index << "\n";
        bm->readData(tableName, offset, index);
    }
   // cout << offset << endl;
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
    cout << "Index " << tableName << "  " << offset << endl;
    int index = bm->getIndex(tableName, offset);
    if(index == -1)
    {
        index = bm->mallocBuffer();
        bm->readData(tableName, offset, index);
    }
    p = bm->retrieveData(index, offset);
    //cout << "No where" << endl;
    if(judger.isSatisfied(p) == true) //&& isAllZero((char *)p, eachSize) == false)
    {
        bm->storeData(index, ch, offset, eachSize);
        //cout << "delete Successfully\n" << "eachSize: " << eachSize << endl;
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
        int index = bm->getIndex(tableName, i*BUFFERSIZE_LIMIT);
        if(index == -1)
            index = bm->mallocBuffer();
        bm->readData(tableName, i*BUFFERSIZE_LIMIT, index);
        for(int j = 0; (j+1)*eachSize < 4096; j++)
        {
            p = bm->retrieveData(index, j*eachSize + i*BUFFERSIZE_LIMIT);
            if(judger.isSatisfied(p) == true)
            {
                //cout << "I'am storing data\n";
                bm->storeData(index, ch, j*eachSize + i*BUFFERSIZE_LIMIT, eachSize);
                fp->addElement(j*eachSize);
            }
            //cout << "Delete Where No Index\n";
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
    filead = tableName + "freeList";
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
    if(table2Freelist.find(tableName) != table2Freelist.end()) return false;
    //cout << "Oh fuck\n";
    string filePath = tableName+"freeList";
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
    // for(int i = 0; i < 4096; i++)
    //    file << buff;
    //file.close();
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
    //cout << "Recorder: read in size: " << size << endl;
    while(k < size)
    {
        file >> tableName >> dataPath >> elementSize;
        //cout << "File in: tableName:" << tableName << "\ndataPath: " <<dataPath << "\nelementSize: " << elementSize << endl;
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
    remove(dataPath.c_str());
    file.open(dataPath, ofstream::out| ios_base::trunc);
    //cout<<"this is the path::"<<dataPath<<endl;
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
    delete bm;
}

bool isIntEqual(int * v1, string& op, string& value)
{
    if(op == "<")
    {
        if(value.find(".") != string::npos)
            return *v1 < std::stoi(value);
        else
            return *v1 < std::stof(value);
    }
    else if(op == ">")
    {
        if(value.find(".") != string::npos)
            return *v1 > std::stoi(value);
        else
            return *v1 > std::stof(value);
    }
    else if(op == "=")
    {
        if(value.find(".") != string::npos)
            return *v1 == std::stoi(value);
        else
            return *v1 == std::stof(value);
    }
    else if(op == "<=")
    {
        if(value.find(".") != string::npos)
            return *v1 <= std::stoi(value);
        else
            return *v1 <= std::stof(value);
    }
    else if(op == ">=")
    {
        if(value.find(".") != string::npos)
            return *v1 >= std::stoi(value);
        else
            return *v1 >= std::stof(value);
    }
    else if(op == "!=")
    {
        if(value.find(".") != string::npos)
            return *v1 != std::stoi(value);
        else
            return *v1 != std::stof(value);
    }
    return false;
}

bool isFloatEqual(float* v1, string& op, string& value)
{
    if(op == "<")
    {
        if(value.find(".") != string::npos)
            return *v1 < std::stoi(value);
        else
            return *v1 < std::stof(value);
    }
    else if(op == ">")
    {
        if(value.find(".") != string::npos)
            return *v1 > std::stoi(value);
        else
            return *v1 > std::stof(value);
    }
    else if(op == "=")
    {
        if(value.find(".") != string::npos)
            return *v1 == std::stoi(value);
        else
            return *v1 == std::stof(value);
    }
    else if(op == "<=")
    {
        if(value.find(".") != string::npos)
            return *v1 <= std::stoi(value);
        else
            return *v1 <= std::stof(value);
    }
    else if(op == ">=")
    {
        if(value.find(".") != string::npos)
            return *v1 >= std::stoi(value);
        else
            return *v1 >= std::stof(value);
    }
    else if(op == "!=")
    {
        if(value.find(".") != string::npos)
            return *v1 != std::stoi(value);
        else
            return *v1 != std::stof(value);
    }
    return false;
}

bool isCharEqual(char* src, string& op, string& value,int len)
{
    int fuck__ = strlen(src);
    fuck__ = min(fuck__, value.length());
    if(op == "<")
    {
        for(int i = 0; i < fuck__; i++)
        {
            if(src[i] >= value.at(i))
                return false;
        }
    }
    else if(op == ">")
    {
        for(int i = 0; i < fuck__; i++)
        {
            if(src[i] <= value.at(i))
                return false;
        }
    }
    else if(op == "=")
    {
        for(int i = 0; i < fuck__ ; i++)
        {
            //cout << "Compare: " << src[i] << " " << value.at(i) << endl;
            if(src[i] != value.at(i))
                return false;
        }
    }
    else if(op == "<=")
    {
        for(int i = 0; i < fuck__; i++)
        {
            if(src[i] > value.at(i))
                return false;
        }
    }
    else if(op == ">=")
    {
        for(int i = 0; i < fuck__; i++)
        {
            if(src[i] < value.at(i))
                return false;
        }
    }
    else if(op == "!=")
    {
        for(int i = 0; i < fuck__; i++)
        {
            if(src[i] == value.at(i))
                return false;
        }
    }
    return true;
}


bool conditionJudge::isSatisfied(char * p)
{
    
    if(this->flag) return true;
    bool flag = false;
    for(int i = 0; i < elementSize; i++)
    {
        if(p[i] != 0)
            flag = true;
    }
    if(flag == false) return false;
    vector<WhereForRecorder>::iterator piter;
    for( piter = cond->begin(); piter != cond->end(); piter++)
    {
        if(piter->type == 0)
        {
            //cout << "FUCK TYPE0\n";
            if(isIntEqual((int*)(p+piter->offset),piter->op,piter->value) == false)
                return false;
        }
        else if(piter->type == 1)
        {
            //cout << "FUCK TYPE1\n";
            if(isFloatEqual((float*)(p+piter->offset),piter->op,piter->value) == false)
                return false;
        }
        else if(piter->type == 2)
        {
            //cout << "FUCK TYPE2" << *p << endl;
            if(isCharEqual((char*)(p+piter->offset),piter->op,piter->value, piter->length) == false)
                return false;
        }
    }
    return true;
}



bool conditionJudge::isSatisfied(void * pt)
{
    if(this->flag == true) return true;
    char * p = (char *)pt;
    bool flag = false;
    for(int i = 0; i < elementSize; i++)
    {
        if(p[i] != 0)
            flag = true;
    }
    if(flag == false) return false;
    vector<WhereForRecorder>::iterator piter;
    for( piter = cond->begin(); piter != cond->end(); piter++)
    {
        if(piter->type == 0)
        {
            if(isIntEqual((int*)(p+piter->offset),piter->op,piter->value) == false)
                return false;
        }
        else if(piter->type == 1)
        {
            if(isFloatEqual((float*)(p+piter->offset),piter->op,piter->value) == false)
                return false;
        }
        else
        {
            if(isCharEqual((char*)(p+piter->offset),piter->op,piter->value, piter->length) == false)
                return false;
        }
    }
    return true;
}
