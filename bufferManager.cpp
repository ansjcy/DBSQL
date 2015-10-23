//
//  bufferManager.cpp
//  sql
//
//  Created by Ah on 10/13/15.
//  Copyright © 2015 Ah. All rights reserved.
//

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
}

void buffer::makeChange(void * newData, short ataSize, short buffOffset)
{
    tag = true;
    memcpy(buff + buffOffset, newData, ataSize);
}

void buffer::writeBackToDisk()
{
    fstream file;
    cout << "Class: Buffer, WriteBcakToDisk.\n";
    file.open(fileAddr);
    file.seekg(fileOffset, ios::beg);
    file.write(buff, BUFFERSIZE_LIMIT);
    file.close();
}

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
    file.seekg(fileOffset, ios::beg);
    file.read(buff, BUFFERSIZE_LIMIT);
    file.close();
}
void* bufferManager::retrieveData(short index, long offset)
{
    vector<int>::iterator p_occu;
    p_occu = find(LRUOccupied, index);
    if(p_occu != LRUOccupied.end())
    {
        LRUOccupied.erase(p_occu);
        LRUOccupied.push_back(index);
    }
    return buff[index].buff + offset;
}

bool bufferManager::isLocked(long index)
{
    return buff[index].pin;
}

bool bufferManager::storeData(short index, void* data, long offset, long size)
{
    
    if(isLocked(index) == true)
    {
        cout << "Buffer is locked! It cannot be rewritten!" << endl;
        return false;
    }
    else if(data == NULL)
    {
        cout << "Invalid data, access NULL!" << endl;
        return false;
    }
    else if(index < 0)
    {
        cout << "Invalid index, index < 0!" << endl;
        return false;
    }
    else if(offset < 0 || offset > BUFFERSIZE_LIMIT)
    {
        cout << "Invalid Offset, offset < 0 or offset > " << BUFFERSIZE_LIMIT << " is not allowed!" << endl;
        return false;
    }
    else if(size + offset > BUFFERSIZE_LIMIT)
    {
        cout << "Size Exceeds limit!" << endl;
        return false;
    }
    else if(addr2Index.count(buff[index].fileAddr  + to_string(buff[index].fileOffset)) != 1)
    {
        cout << "Invalid operation!" << endl;
        return false;
    } 
    else
    {
        setTag(index);
        memcpy(buff[index].buff + offset, data, size);
        return true;
    }
}

bufferManager::bufferManager()
{
    for(int i = 0; i < BUFFERNUM_LIMIT; i++)
    {
       // cout << i << endl;
        LRUFree.push_back(i);
    }
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
        LRULocked.erase(find(LRULocked, index));
        LRUOccupied.push_back(index);
        buff[index].pin = false;
    }
}

void bufferManager::setTag(long index)
{
    //表明block中内容从初始化之后被改变过
    buff[index].tag = true;
}

void bufferManager::readData(string& fileAddr, long offset)
{
   /*
    * 首先判断是否已经读取, 其次, 判断申请是否成功
    */
    map<string, int>::iterator iter;
    iter = addr2Index.find(fileAddr + to_string(offset));
    cout << "ReadData: " << fileAddr + to_string(offset) << endl;
    if(iter == addr2Index.end())
    {
        long index = mallocBuffer();
        if(index >= 0)
        {
            addr2Index.insert(pair<string,int>(fileAddr+to_string(offset),index));
            buff[index].readDataFromDisk(fileAddr, offset);
            cout << "Read Data finished in index: " << index << endl;
        }
    }
}

long bufferManager::mallocBuffer()
{
    if(LRUFree.empty() == true && LRUOccupied.empty() == true)
    {
        //如果所有的block都被锁定，则申请失败
        cout << "mallocBuffer: all are locked(0)\n";
        return -1;       
    }
    else if( LRUFree.empty() == false)
    {
        //有未被占用的block
        cout << "MallocBuffer: Okay(1) \n";
        int index = LRUFree.front();
        LRUFree.erase(LRUFree.begin());
        LRUOccupied.push_back(index);
        return index;
    }
    else
    {
        //无未被占用的block，但是并非所有都被锁定，则进行LRU替换。
        cout << "MallocBuffer: Okay(2)\n";
        int index = LRUOccupied.front();
        LRUOccupied.erase(LRUOccupied.begin());
        LRUOccupied.push_back(index);
        buff[index].initial();
        return index;
    }
}

void bufferManager::freeBuffer(short index)
{
    addr2Index.erase(buff[index].fileAddr + to_string(buff[index].fileOffset));
    vector<int>::iterator p_lock, p_free, p_occu;
    p_lock = find(LRULocked, index);
    p_occu = find(LRUOccupied, index);
    p_free = find(LRUFree, index);
    if(buff[index].tag == true)
    {
        buff[index].writeBackToDisk();
    }
    if(p_occu != LRUOccupied.end())
        LRUOccupied.erase(p_occu);
    else if(p_lock != LRULocked.end())
        LRULocked.erase(p_lock);
    buff[index].initial();
    if(p_free == LRUFree.end())
        LRUFree.push_back(index);
    cout << "FreeBuffer: Succeed to free buffer\n";
}
