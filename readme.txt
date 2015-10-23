关于bufferManager
buffer读取跨度以4kb 为单位。
即文件的一个4kb， 第二个4kb，以此类推。



1.如何获取一个block存储?
    1.1 通过mallocBuffer() 获取一个单元。默认不加锁。返回index ;
    1.2 通过readData(string & fileAddr, long offset, long index)读取磁盘数据;
        buffer是介于磁盘与程序之间的内存块，所以每个buffer块有:
        string fileAddr, long offset. 对应于磁盘中。
    1.3 如果需要修改内容，使用storeData(short index, void* data, long offset, long size)
        这里说明， index用来索引block, data表示所要写入数据初始地址，offset表示所要写入block的初始地址，szie表示所要写入内容长度(单位byte)
    1.4 如果为了防止所获得的block被LRU算法替换（比如：B+树的节点），可以进行lock，即调用：
        lock(long index). 注意：lock的buffer必须是已经被malloc的，否则不会进行加锁！
    1.5 如果想要释放某个block, 使用freeBuffer(short index)即可。

2.函数功能具体说明:
    2.1 bufferManager()
        此函数为bufferManager类的构造函数，主要为初始化
        其中，将所有的可用的block加入到LRUFree（vector<int>）中。
    2.2 mallocBuffer()
        申请一个新的block,返回index， 如果：
        1). 所有的block均被锁定，则返回－1，表示申请失败。
        2). 如果存在空闲的block, 则返回对应的index, 并且从LRUFree中删除，同时加入到LRUOccupied中
        3). 如果不存在空闲额block，并且有未被占用的block, 则交出LRUOccupied中的第一个block,同时
            从LRUOccupied中删除，并且加入到队尾。
    2.3 readData(string& file, long offset, long index)
        读取数据到index索引到的buffer中.
    2.4 lock(long index)
        锁定由index索引到的block。（当且仅当该block存在于LRUOccupied队列中）
        同时，讲其从LRUOccupied队列中删除，并且加入到LRULocked队列中
    2.5 unlock(long index)
        当且仅当该block在LRULoced队列中，从中删除并加入到LRUOccupied队列末尾。
    2.6 isLocked(long index)
        返回该block是否被锁定
    2.7 steTag(long index)
        设置标记，表明该block被修改过
    2.8 storeData(short index, void* data, long offset, long size)
        为了能够将新的数据写入磁盘，于是先将其所属文件块（4k大小）读入。
        然后，将内容插入其中。并更新LRU
    2.9 freeBuffer(short index)
        将buffer释放，并且如果有修改就写回原文件，否则直接从其它队列中删除，并加入到自由队列中。
    2.10 retrieveData(short index, long offset)
        取回数据，并更新LRU。

        
