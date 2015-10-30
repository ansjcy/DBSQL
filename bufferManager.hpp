//
//  bufferManager.hpp
//  sql
//
//  Created by Ah on 10/13/15.
//  Copyright © 2015 Ah. All rights reserved.
//

#ifndef bufferManager_hpp
#define bufferManager_hpp


#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <sstream>
using namespace std;
#define BUFFERSIZE_LIMIT 4096
#define BUFFERNUM_LIMIT 1024
/*
 class buffer
 负责： 1. 从磁盘读取数据， 参数：磁盘地址（即文件地址），文件偏移，数据量大小
 2. 标记，buffer内容是否被修改？buffer是否被锁定，无法替换。
 3. 将数据写回磁盘
 4. 动态数据写入buffer（比如B+树新生成的节点）
 5. 只有通过bufferManager 才可与buffer交互。
 */
//assume each record is 



class buffer
{
private:
    char buff[BUFFERSIZE_LIMIT];     //to store the data
    string fileAddr;                 //mark the address of the data in the disk
    long fileOffset;                 //mark the  offset of the data.
    bool tag;                        //0-no changes 1-changed
    bool pin;
    //short elementSize;

    void readDataFromDisk(string& fileAddr, long fileOffset);//, short elementSize);
    void initial();
    void writeBackToDisk();
    //void readDataFromMem(void * dataAddr, short ataSize, string & fileAddr, long fileOffset);
    //void makeChange(void * newData, short ataSize, short buffOffset);
    friend class bufferManager;
    buffer();
};


/*
 class bufferManager
 负责：
 1. 管理buffer，分配缓存。
 2. 释放缓存。
 3. 修改buffer。
 4. 读取buffer数据。
 5. 只有通过bufferManager 才可与buffer交互。
 */

class bufferManager
{
private:
    buffer buff[BUFFERNUM_LIMIT];
    map<string, int> addr2Index;
    vector<int> LRUFree;
    vector<int> LRUOccupied;
    vector<int> LRULocked;
public:
    bufferManager();        //finished
    ~bufferManager();
    void lock(long index);  //finished
    void unlock(long index); //finished
    bool isLocked(long index);//finished
    void setTag(long index); //finished
    void readData(string& fileAddr, long offset, long index);  //从磁盘读 finished
    long getIndex(string& fileAddr, long offset);
    long mallocBuffer();                          //(finished)返回buffer下标 0～BUFFERNUM_LIMIT
    bool storeData(short index, void* data, long offset, long size); //finish
    void freeBuffer(short index, bool choice = true);                  //finished
    //bool reWriteBuffer(short index, void* data, long offset, long size);
    void* retrieveData(short index, long offset);
};



#endif /* bufferManager_hpp */
