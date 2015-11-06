// Catalog Manager
// Bohan Li

#ifndef CATALOG_MANAGER_
#define CATALOG_MANAGER_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string.h>

class AttrInfo {
public:
    AttrInfo() {indexRootOffset = 0;}
    AttrInfo(const std::string& _name, int _typeId, int _strLen, bool _isUnique, bool _isPrimaryKey, bool _hasIndex) : name(_name), typeId(_typeId), strLen(_strLen), isUnique(_isUnique), isPrimaryKey(_isPrimaryKey), hasIndex(_hasIndex) {indexRootOffset = 0;}
    
    void setName(const std::string& _name) { name = _name; }
    void setTypeId(int _typeId) { typeId = _typeId; }
    void setStrLen(int _strLen) { strLen = _strLen; }
    void setIsUnique(bool _isUnique) { isUnique = _isUnique; }
    void setIsPrimaryKey(bool _isPrimaryKey) { isPrimaryKey = _isPrimaryKey; }
    void setHasIndex(bool _hasIndex) { hasIndex = _hasIndex; }
    
    std::string getName() const { return name; }
    int getTypeId() const { return typeId; }
    int getStrLen() const { return strLen; }
    bool getIsUnique() const { return isUnique; }
    bool getIsPrimaryKey() const { return isPrimaryKey; }
    bool getHasIndex() const { return hasIndex; }
    
    int indexRootOffset;
    
private:
    std::string name;
    int typeId; // 0: int, 1: float, 2: string
    int strLen;
    bool isUnique;
    bool isPrimaryKey;
    bool hasIndex;
};




class TableInfo {
public:
    TableInfo() {};
    TableInfo(const std::string& _name) : name(_name) {};
    
    void setName(const std::string& _name) { name = _name; }
    void addAttr(const std::string& _attrName, const AttrInfo& _attrInfo) {
        attrNames.push_back(_attrName);
        attrs[_attrName] = _attrInfo;
    }
    
    void removeAttr(const std::string& _attrName, const AttrInfo& _attrInfo) {
        std::vector<std::string>::iterator it;
        for( it = attrNames.begin(); it != attrNames.end(); it++ ) {
            if( *it == _attrName ) {
                attrNames.erase(it);
            }
        }
        
        attrs[_attrName] = _attrInfo;
    }
    
    std::string getName() const { return name; }
    std::vector<std::string> getAttrNames() const {
        std::vector<std::string> rst(attrNames);
        return rst;
    }
    AttrInfo getAttr(std::string _attrName) {
        std::map<std::string, AttrInfo> tmpMap(attrs);
        return tmpMap[_attrName];
        
    }
    std::map<std::string, AttrInfo> getAllAttrs() const { return attrs; }
    
    std::string getPrimaryKey() const {
        std::map<std::string, AttrInfo> tmpMap(attrs);
        std::map<std::string, AttrInfo>::iterator it;
        for( it = tmpMap.begin(); it != tmpMap.end(); it++ ) {
            if( it->second.getIsPrimaryKey() ) {
                return it->first;
            }
        }
        return "";///////////
    }
    
    std::vector<std::string> getIndex() const {
        std::vector<std::string> indexes;
        std::map<std::string, AttrInfo> tmpMap(attrs);
        std::map<std::string, AttrInfo>::iterator it;
        for( it = tmpMap.begin(); it != tmpMap.end(); it++ ) {
            if( it->second.getHasIndex() ) {
                indexes.push_back(it->first);
            }
        }
        
        return indexes;
    }
    
    std::map<std::string, std::string> indexList;
    // index_name - attrbute_type pair

private:
    std::string name;
    std::vector<std::string> attrNames;
    std::map<std::string, AttrInfo> attrs;
};

class CatalogManager {
public:
    // deal with table
    void createTableInfo(const TableInfo& tableInfo);
    void dropTableInfo(std::string tableName);
    TableInfo getTableInfo(const std::string& tableName);
    void writeTableInfo(const TableInfo& tableInfo);
    
    //what i need...
    bool hasTable(std::string tableName){return true;}
};

#endif