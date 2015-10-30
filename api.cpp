//
//  api.cpp
//  MiniSQL
//
//  Created by jason on 10/18/15.
//  Copyright © 2015 jason. All rights reserved.
//

#include "api.hpp"

string file_addr = "/Users/jason/Desktop/";
extern recorder rm;
extern CatalogManager cm;
extern IndexManager im;

void API::chooseCommand(AUX* command)
{
    string type = command->getType();
    if(type == "not valid")
    {
        isSucceed = false;
        //information = "there is an error in the command!";
        cout<<command->getInformation();
    }
    else if(type == "create table")
    {
        AUXCreateTable* cast_command = static_cast<AUXCreateTable*>(command);
        createTable(cast_command);
        cout<<cast_command->getInformation();
    }
    else if(type == "drop table")
    {
        AUXDropTable* cast_command = static_cast<AUXDropTable*>(command);
        dropTable(cast_command);
        cout<<cast_command->getInformation();
    }
    else if(type == "create index")
    {
        AUXCreateIndex* cast_command = static_cast<AUXCreateIndex*>(command);
        createIndex(cast_command);
        cout<<cast_command->getInformation();
    }
    else if(type == "drop index")
    {
        AUXDropIndex* cast_command = static_cast<AUXDropIndex*>(command);
        dropIndex(cast_command);
        cout<<cast_command->getInformation();
    }
    else if(type == "insert into")
    {
        AUXInsertInto* cast_command = static_cast<AUXInsertInto*>(command);
        InsertInto(cast_command);
        cout<<cast_command->getInformation();
    }
    else if(type == "delete from")
    {
        AUXDeleteFrom* cast_command = static_cast<AUXDeleteFrom*>(command);
        deleteFrom(cast_command);
        cout<<cast_command->getInformation();
    }
    else if(type == "select from")
    {
        AUXSelectFrom* cast_command = static_cast<AUXSelectFrom*>(command);
        selectFrom(cast_command);
        cout<<cast_command->getInformation();
    }
    
}


//void API::setInformation(string info)
//{
//    information = info;
//}
//void API::printInformation(AUX* command)
//{
//    if(isSucceed == false)
//    {
//        cout<<command->;
//        return;
//    }
//    else
//        cout<<"success";
//}
void API::setSucceed(bool success)
{
    isSucceed = success;
}
/*
 map<string, string> attributes;
 map<string, int> attributes_length;
 vector<string> attributes_order;
 
 string primary_key;
 vector<string> unique;
 */


void API::createTable(AUXCreateTable* cast_command)
{
    TableInfo table_info;
    AttrInfo attr_info;
    string table_name = cast_command->getTableName();
    string primary_key = cast_command->getPrimaryKey();
    vector<string>uniques = cast_command->getUniqueKeys();
    string tmpFileAddr = file_addr + table_name;
    //recorder rm(file_addr);
    
    //@@@@@@@hastable, mind this function..
//    if (cm.hasTable(table_name)){
//        setSucceed(false);
//        cast_command->setInformation("The table has existed!");
//        return;
//    }
    
    table_info.setName(table_name);
    map<string, string>attrs = cast_command->getAttributes();
    map<string, int>attrs_length = cast_command->getAttributesLength();
    map<string, string>::iterator it;
    map<string, int>::iterator itLen;
    short element_size = 0;
    for (it = attrs.begin(), itLen = attrs_length.begin(); it != attrs.end(); it++, itLen++) {
        attr_info.setName(it->first);
        attr_info.setTypeId((it->second == "int")?0:(it->second == "float")?1:(it->second == "char")?2:99);
        element_size+=itLen->second;
        attr_info.setStrLen(itLen->second);
        //is primary key
        if(it->first == primary_key)
        {
            attr_info.setIsPrimaryKey(true);
        }
        //is unique
        for(int i = 0; i < uniques.size(); i++)
        {
            if(uniques[i] == it->first)
            {
                attr_info.setIsUnique(true);
                break;
            }
        }
        //add attr
        table_info.addAttr(it->first, attr_info);
    }
    cm.createTableInfo(table_info);
    rm.createTable(tmpFileAddr, element_size);
}


void API::dropTable(AUXDropTable* cast_command)
{
    TableInfo table_info;
    AttrInfo attr_info;
    string table_name = cast_command->getTableName();
    
    //table exists? if not, return
    if(!cm.hasTable(table_name))
    {
        setSucceed(false);
        cast_command->setInformation("there is no such table!");
        return;
    }
    //get the information of this table
    table_info = cm.getTableInfo(table_name);
    map<string, AttrInfo> attrs = table_info.getAllAttrs();
    vector<string> indexes = table_info.getIndex();
    //if has indexes, drop these indexes. if failed, return
    if(!indexes.empty())
    {
        //@@@@@@drop index!
    }
    //@@@@@@drop table, if success, return. failed, return..
    
}


void API::createIndex(AUXCreateIndex* cast_command)
{
    TableInfo table_info;
    AttrInfo attr_info;
    string table_name = cast_command->getTableName();
    string index_name = cast_command->getIndexName();
    string attr_name = cast_command->getAttributeName();

    //table exists? if not , return
    if(!cm.hasTable(table_name))
    {
        setSucceed(false);
        cast_command->setInformation("there is no such table!");
        return;
    }
    
    table_info = cm.getTableInfo(table_name);
    map<string, AttrInfo> attrs = table_info.getAllAttrs();
    map<string, AttrInfo>::iterator it;
    
    //attribute exist? if not, return
    it = attrs.find(attr_name);
    if(it == attrs.end())
    {
        setSucceed(false);
        cast_command->setInformation("there is no such attr!");
        return;
    }

    //index exists? if so, return
    if(attrs[attr_name].getHasIndex())
    {
        setSucceed(false);
        cast_command->setInformation("this attr has had index!");
        return;
    }
    
    //attribute unique? if not, return
    if(!attrs[attr_name].getIsUnique())
    {
        setSucceed(false);
        cast_command->setInformation("this attr is not unique!");
        return;
    }
    //@@@@@@create index and add records..
    
    //needs im.addrecord and cm.addindex..
}


void API::dropIndex(AUXDropIndex* cast_command)
{
    TableInfo table_info;
    AttrInfo attr_info;
    string table_name = cast_command->getTableName();
    string index_name = cast_command->getIndexName();
    
    //table exists? if not , return
    if(!cm.hasTable(table_name))
    {
        setSucceed(false);
        cast_command->setInformation("there is no such table!");
        return;
    }
    
    vector<string> indexes = table_info.getIndex();
    //index exists??
    if(indexes.empty())
    {
        setSucceed(false);
        cast_command->setInformation("there is no such index!");
        return;
    }
    int i;
    for(i = 0; i < indexes.size(); i++)
    {
        if(indexes[i] == index_name)
        {
            break;
        }
    }
    if(i == indexes.size())
    {
        setSucceed(false);
        cast_command->setInformation("there is no such index!");
        return;
    }
    //drop index and updata table info..
    
}


void API::InsertInto(AUXInsertInto* cast_command)
{
    TableInfo table_info;
    AttrInfo attr_info;
    string table_name = cast_command->getTableName();
    char dt[4096];
    int bias = 0;
    //table exists? if not , return
    if(!cm.hasTable(table_name))
    {
        setSucceed(false);
        cast_command->setInformation("there is no such table!");
        return;
    }
    
    table_info = cm.getTableInfo(table_name);
    vector<string> attrs = table_info.getAttrNames();
    vector<string> values;
    
    //has input the attrs
    if(!cast_command->getAttributeNames().empty())
    {
        //check is the same length
        if(cast_command->getAttributeNames().size() != cast_command->getValues().size())
        {
            setSucceed(false);
            cast_command->setInformation("the number of values is not the same with the number of attrs");
            return;
        }
        //sort the values
        for(int j = 0; j < attrs.size(); j++)
        {
            int i;
            for(i = 0; i < cast_command->getAttributeNames().size(); i++)
            {
                if(cast_command->getAttributeNames()[i] == attrs[j])
                {
                    values.push_back(cast_command->getValues()[i]);
                    break;
                }
            }
            if(i == cast_command->getAttributeNames().size())
            {
                setSucceed(false);
                cast_command->setInformation("there is no such attr!!");
                return;
            }
        }
    }
    else
    {
        values = cast_command->getValues();
    }
    //check overflow
    for(int i = 0; i < values.size(); i++)
    {
        int type_id = table_info.getAttr(attrs[i]).getTypeId(); //0:int, 1:float, 2:string.
        int str_length = table_info.getAttr(attrs[i]).getStrLen();
        if(type_id == 0) //int
        {
            if(!isInt(values[i]))
            {
                setSucceed(false);
                cast_command->setInformation("is is not a integer");
                return;
            }
            try
            {
                int tmp = stoi(values[i]);
                memcpy(dt+bias, &tmp, sizeof(int));
                bias+=sizeof(int);
            }
            catch(out_of_range error){
                setSucceed(false);
                cast_command->setInformation("integer out of bounds");
                return;
            }
        }
        else if(type_id == 2)  //float
        {
            if(!isFloat(values[i]))
            {
                setSucceed(false);
                cast_command->setInformation("is is not a float number");
                return;
            }
            try
            {
                float tmp = stof(values[i]);
            }
            catch(out_of_range error){
                setSucceed(false);
                cast_command->setInformation("float out of bounds");
                return;
            }
        }
        else if(type_id == 3)  //char
        {
            if(values[i].length() > str_length)
            {
                setSucceed(false);
                cast_command->setInformation("char out of bounds");
                return;
            }
        }
        
        
    }
    //check inserting into a unique attr (or primary)
        //use recorder..
    //insert and update index..
    long offset = rm.insert(<#void *data#>, <#string &tableName#>);
}


void API::selectFrom(AUXSelectFrom* cast_command)
{
    TableInfo table_info;
    AttrInfo attr_info;
    string table_name = cast_command->getTableName();
    vector<string> attr_selected;
    
    //@@@@@@@hastable, mind this function..
    if (cm.hasTable(table_name)){
        setSucceed(false);
        cast_command->setInformation("The table has existed!");
        return;
    }
    table_info = cm.getTableInfo(table_name);
    if(cast_command->getIsAllValues())
    {
        attr_selected = table_info.getAttrNames();
    }
    else{
        attr_selected = cast_command->getAttributes();
        //if the attr exists..
        for(int i = 0; i < attr_selected.size(); i++)
        {
            int j;
            for(j = 0; j < table_info.getAttrNames().size(); j++)
            {
                if(attr_selected[i] == table_info.getAttrNames()[j])
                    break;
            }
            if(j == table_info.getAttrNames().size())
            {
                setSucceed(false);
                cast_command->setInformation("there is no such attr..");
                return;
            }
        }
    }
    //check whether the where sentence is valid
    vector<Where> where = cast_command->getConditions();
    for(int i = 0; i < where.size(); i++)
    {
        int j;
        for(int j = 0; j < table_info.getAttrNames().size(); j++)
        {
            if(where[i].attribute == table_info.getAttrNames()[j])
                break;
        }
        if(j == table_info.getAttrNames().size())
        {
            setSucceed(false);
            cast_command->setInformation("there is no such attr in the where sentence");
            return;
        }
    }
    //insert..
    //with some index
        //get index name, get offset, get data..
    //with no index
        //get data
    
    //deal with the same data..
    //print out the table
    
}


void API::deleteFrom(AUXDeleteFrom* cast_command)
{
    TableInfo table_info;
    AttrInfo attr_info;
    string table_name = cast_command->getTableName();
    vector<string> attr_selected;
    
    //@@@@@@@hastable, mind this function..
    if (cm.hasTable(table_name)){
        setSucceed(false);
        cast_command->setInformation("The table has existed!");
        return;
    }
    table_info = cm.getTableInfo(table_name);
    
    vector<Where> where = cast_command->getConditions();
    for(int i = 0; i < where.size(); i++)
    {
        int j;
        for(int j = 0; j < table_info.getAttrNames().size(); j++)
        {
            if(where[i].attribute == table_info.getAttrNames()[j])
                break;
        }
        if(j == table_info.getAttrNames().size())
        {
            setSucceed(false);
            cast_command->setInformation("there is no such attr in the where sentence");
            return;
        }
    }
    if (cast_command->getIsDeleteAll()) {
        //delete in recorder
        //update index
    }
    else{
        
    }
    //has index
    //has no index..
    //update index..
}
