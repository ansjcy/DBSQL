//
//  indexManager.hpp
//  MiniSQL
//
//  Created by jason on 10/26/15.
//  Copyright © 2015 jason. All rights reserved.
//

#ifndef indexManager_hpp
#define indexManager_hpp

#include <stdio.h>
#include "auxiliary.hpp"
#include "b_plus_tree.h"
#include "bufferManager.hpp"
#include "recorder.hpp"
class IndexManager {
public:
    //BufferManager bfManager;
    BPlusTree btree;
    std::string filename;
    BufferManager bufferManager;
    
    IndexManager(recorder* r) {
        //bufferManager.setRecorder(r);
    }
    
    int createIndex(std::string filename, int typeId = 0, int strLen = 0) {
//        recorder* r;
//        r = bufferManager.getRecorder();
//        r->createTable(filename, BLOCK_SIZE);
//        
//        this->filename = filename;
//        btree.setBufferManager(&bufferManager);
//        btree.setFilename(filename);
//        Node rootNode(true, true);
//        Block rootBlock = bufferManager.newBlock(filename);
//        btree.setTypeId(typeId);
//        btree.setStrLen(strLen);
//        btree.setRootNode(rootNode);
//        btree.setRootPos(rootBlock.pos);
//        rootNode.write2Blk(rootBlock, btree.getTypeId(), btree.getStrLen(), &bufferManager);
//        
//        return rootBlock.pos;
        return 0;
    }
    
    int getIndex(std::string filename, int rootPos, int typeId = 0, int strLen = 0){
//        recorder* r;
//        r = bufferManager.getRecorder();
//        
//        this->filename = filename;
//        btree.setBufferManager(&bufferManager);
//        btree.setFilename(filename);
//        
//        Block rootBlock = bufferManager.getBlock(filename, rootPos);
//        Node rootNode(rootBlock);
//        btree.setTypeId(typeId);
//        btree.setStrLen(strLen);
//        btree.setRootNode(rootNode);
//        btree.setRootPos(rootBlock.pos);
//        
//        return rootBlock.pos;
        return 0;
    }
    /////////typeId = 0...
    void insertIntoIndex(std::map<AttrType, int> datalist, int typeId = 0) {
        std::map<AttrType, int>::iterator it;
        for( it = datalist.begin(); it != datalist.end(); it++ ) {
            std::cout << it->first.datai << " " << it->second << std::endl;
            btree.insert(it->first, it->second);
            
        }
    }
    
    std::vector<int> findInIndex(std::vector<AttrType> datalist) {
        std::vector<int> rst;
        
        std::vector<AttrType>::iterator it;
        for( it = datalist.begin(); it != datalist.end(); it++ ) {
            int tmpRst;
            //tmpRst = btree._findLeaf(*it);
            tmpRst = btree.find(*it);
            //std::cout << (*it).datai << "-->" << tmpRst << std::endl;
            rst.push_back(tmpRst);
        }
        return rst;
    }
    
    void deleteFromIndex(std::vector<AttrType> datalist) {
        std::vector<int> rst;
        
        std::vector<AttrType>::iterator it;
        for( it = datalist.begin(); it != datalist.end(); it++ ) {
            btree.remove(*it);
        }   
    }
    
    void dropIndex(std::string filename) {
        
    }
};
#endif /* indexManager_hpp */
