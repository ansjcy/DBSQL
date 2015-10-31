//
//  bufferManager.cpp
//  sql
//
//  Created by Ah on 10/13/15.
//  Copyright © 2015 Ah. All rights reserved.
//

/*
 
 
 
 */

#include "bufferManager.hpp"
vector<int>::iterator find(vector<int> & vec, long index)
{
    vector<int>::iterator p;
    for(p = vec.begin(); p != vec.end(); p++)
    {
        if(*p == index)
            return p;
    }
    return vec.end();
}

/*
 * buffer constructor
 * set tag = false, no change made.
 */
buffer::buffer()
{
    initial();
}

void buffer::initial()
{
    pin = false;
    tag = false;
    for(int i = 0; i < BUFFERSIZE_LIMIT; i++)
        buff[i] = 0;
}
/*
 void buffer::makeChange(void * newData, short ataSize, short buffOffset)
 {
 tag = true;
 memcpy(buff + buffOffset, newData, ataSize);
 }
 */
void buffer::writeBackToDisk()
{
    fstream file;
    //cout << "Class: Buffer, WriteBcakToDisk.\n";
    
    file.open(fileAddr); //ofstream::out|ios_base::app);
    file.seekg(0,ios::end);
    long fsize = 0;
    fsize = file.tellg();
    file.close();
    if(fsize > fileOffset*BUFFERSIZE_LIMIT)
        file.open(fileAddr, ofstream::out);
    else
        file.open(fileAddr, ofstream::out|ios_base::app);
    //cout << "fileOffset: " << fileOffset << endl;
    //file.seekg(0,ios::end);
    //long fsize = file.tellg();
    //if(fsize > fileOffset*BUFFERSIZE_LIMIT)
    file.seekg(fileOffset*BUFFERSIZE_LIMIT, ios::beg);
    //else
    //    file.seekg(0, ios::end);
    file.write(buff, BUFFERSIZE_LIMIT);
    file.close();
}

/*
 void buffer::readDataFromMem(void * dataAddr, short ataSize, string & fileAddr, long fileOffset)
 {
 memcpy(buff, dataAddr, ataSize);
 this->fileAddr = fileAddr;
 this->fileOffset = fileOffset;
 fstream file;
 file.open(fileAddr);
 file.seekg(fileOffset, ios::beg);
 file.read(buff + ataSize, BUFFERSIZE_LIMIT - ataSize);
 file.close();
 }
 */
/*
 *This function is to read the dada from the disk.
 *You should give the file address and the offset.
 */
void buffer::readDataFromDisk(string& fileAddr, long fileOffset)
{
    this->fileAddr = fileAddr;
    this->fileOffset = fileOffset;
    //this->elementSize = elementSize;
    fstream file;
    file.open(fileAddr);
    file.seekg(fileOffset*BUFFERSIZE_LIMIT, ios::beg);
    file.read(buff, BUFFERSIZE_LIMIT);
    file.close();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void* bufferManager::retrieveData(short index, long offset) //块内偏移
{
    offset = offset - offset/4096*4096;
    vector<int>::iterator p_occu;
    p_occu = find(LRUOccupied, index);
    if(p_occu != LRUOccupied.end())
    {
        LRUOccupied.erase(p_occu);
        LRUOccupied.push_back(index);
    }
    void * p = (void*)(buff[index].buff + offset);
    return p;//buff[index].buff + offset;
}

bool bufferManager::isLocked(long index)
{
    return buff[index].pin;
}

bool bufferManager::storeData(short index, void* data, long offset, long size)
{
    //offset - 块内偏移
    offset = offset - offset/4096*4096;
    if(isLocked(index) == true)
    {
        //cout << "Buffer is locked! It cannot be rewritten!" << endl;
        return false;
    }
    else if(data == NULL)
    {
        //cout << "Invalid data, access NULL!" << endl;
        return false;
    }
    else if(index < 0)
    {
        //cout << "Invalid index, index < 0!" << endl;
        return false;
    }
    else if(offset < 0 || offset > BUFFERSIZE_LIMIT)
    {
        //cout << "Invalid Offset, offset < 0 or offset > " << BUFFERSIZE_LIMIT << " is not allowed!" << endl;
        return false;
    }
    else if(size + offset > BUFFERSIZE_LIMIT)
    {
        //cout << "Size Exceeds limit!" << endl;
        return false;
    }
    else if(addr2Index.count(buff[index].fileAddr  + to_string(buff[index].fileOffset)) != 1)
    {
        //cout << "Invalid operation!" << endl;
        return false;
    }
    else
    {
        setTag(index);
        //cout << "In storeData: " << offset << endl;
        memcpy(buff[index].buff + offset, data, size);
        return true;
    }
}

bufferManager::bufferManager()
{
    for(int i = 0; i < BUFFERNUM_LIMIT; i++)
    {
        LRUFree.push_back(i);
    }
    //cout << "Construct BufferManager Successfully!" << endl;
}

void bufferManager::lock(long index)
{
    //如果block已经是被占用的就可以加锁，否则不行, 并且从占用队列中删除
    //，同时加入到加锁队列中
    vector<int>::iterator p_occu;
    p_occu = find(LRUOccupied, index);
    if(p_occu != LRUOccupied.end())
    {
        buff[index].pin = true;
        LRUOccupied.erase(p_occu);
        LRULocked.push_back(index);
    }
}

void bufferManager::unlock(long index)
{
    
    //如果block已经是被加锁的，那么就从加锁队列中删除
    //并且同时加入占用队列中
    vector<int>::iterator p_lock = find(LRULocked, index);
    if(p_lock != LRULocked.end())
    {
        LRULocked.erase(p_lock);
        LRUOccupied.push_back(index);
        buff[index].pin = false;
    }
}

void bufferManager::setTag(long index)
{
    //表明block中内容从初始化之后被改变过
    buff[index].tag = true;
}

void bufferManager::readData(string& fileAddr, long offset, long index)
{
    /*
     * 首先判断是否已经读取, 其次, 判断申请是否成功
     * 全局偏移
     */
    map<string, int>::iterator iter;
    iter = addr2Index.find(fileAddr + to_string(offset/BUFFERSIZE_LIMIT));
    //cout << "ReadData: " << fileAddr + to_string(offset) << endl;
    if(iter == addr2Index.end())
    {
        //long index = mallocBuffer();
        if(index >= 0)
        {
            addr2Index.insert(pair<string,int>(fileAddr+to_string(offset/BUFFERSIZE_LIMIT),index));
            buff[index].readDataFromDisk(fileAddr, offset/BUFFERSIZE_LIMIT);
            //cout << "Read Data finished in index: " << index << buff[index].buff[231] <<  buff[index].buff[232] << endl;
        }
    }
}

long bufferManager::mallocBuffer()
{
    if(LRUFree.empty() == true && LRUOccupied.empty() == true)
    {
        //如果所有的block都被锁定，则申请失败
        //cout << "mallocBuffer: all are locked(0)\n";
        return -1;
    }
    else if( LRUFree.empty() == false)
    {
        //有未被占用的block
        //cout << "MallocBuffer: Okay(1) \n";
        int index = LRUFree.front();
        LRUFree.erase(LRUFree.begin());
        LRUOccupied.push_back(index);
        return index;
    }
    else
    {
        //无未被占用的block，但是并非所有都被锁定，则进行LRU替换。
        //cout << "MallocBuffer: Okay(2)\n";
        int index = LRUOccupied.front();
        freeBuffer(index);
        return LRUFree.front();
    }
}
long bufferManager::getIndex(string& fileAddr, long offset)
{
    //BLOCK NUMBER
    offset = offset/BUFFERSIZE_LIMIT;
    
    map<string, int>::iterator iter;
    iter = addr2Index.find(fileAddr + to_string(offset));
    if(iter == addr2Index.end())
        return -1;
    else
        return iter->second;
    
}
void bufferManager::freeBuffer(short index, bool choice)
{
    addr2Index.erase(buff[index].fileAddr + to_string(buff[index].fileOffset));
    vector<int>::iterator p_lock, p_free, p_occu;
    p_lock = find(LRULocked, index);
    p_occu = find(LRUOccupied, index);
    p_free = find(LRUFree, index);
    if(buff[index].tag == true && choice)
        buff[index].writeBackToDisk();
    if(p_occu != LRUOccupied.end())
        LRUOccupied.erase(p_occu);
    else if(p_lock != LRULocked.end())
        LRULocked.erase(p_lock);
    buff[index].initial();
    if(p_free == LRUFree.end())
        LRUFree.push_back(index);
    //cout << "FreeBuffer: Succeed to free buffer\n";
}

bufferManager::~bufferManager()
{
    map<string,int>::iterator its;
    vector<int> index;
    //cout << "Size of Map: " << addr2Index.size() << endl;
    for(its = addr2Index.begin(); its != addr2Index.end(); ++its)
        index.push_back(its->second);
    for(vector<int>::iterator p = index.begin(); p != index.end(); p++)
        freeBuffer(*p);
    
}
