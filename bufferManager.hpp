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
#include <vector>
using namespace std;

/*
 class buffer
 负责： 
 1. 从磁盘读取数据， 参数：磁盘地址（即文件地址），文件偏移，数据量大小
 2. 标记，buffer内容是否被修改？buffer是否被锁定，无法替换。
 3. 将数据写回磁盘
 4. 动态数据写入buffer（比如B+树新生成的节点）
 5. 只有通过bufferManager 才可与buffer交互。
 */

class buffer
{
private:
    char buff[4096];
    string fileAddr;
    long fileOffset;
    bool tag;                       //0-no changes 1-changed
    char repTag;                    //2-can be replace 1-recently used 0-locked
    void readDataFromDisk(string fileAddr, long fileOffset, long size = 4096);
    void setTag();
    void writeBackToDisk();
    void readDataFromMem(void * dataAddr, long ataSize, long bufferOffset);
    friend class bufferManager;
    
    
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
    buffer buff[1024];
public:
    void readData(string fileAddr, long offset, long size = 4096);
    short mallocBuffer();                                           //返回buffer下标 0～1023
    bool storeData(short index, void* data, long offset, long size);
    bool freeBuffer(short index);
    bool reWriteBuffer(short index, void* data, long offset, long size);
    void* retrieveData(short index, long offset);
};

#endif /* bufferManager_hpp */
