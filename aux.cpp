//
//  aux.cpp
//  MiniSQL
//
//  Created by jason on 10/11/15.
//  Copyright © 2015 jason. All rights reserved.
//

#include "aux.hpp"
//-------------create table---------
void AUXCreateTable::setAttributes(string attr_name, string attr_type, int length)
{
    attributes[attr_name] = attr_type;
    attributes_length[attr_name] = length;
    attributes_order.push_back(attr_name);
}
void AUXCreateTable::setPrimaryKeys(string key)
{
    primary_key = key;
}
void AUXCreateTable::setUnique(string unique_key)
{
    unique.push_back(unique_key);
}
map<string, string> AUXCreateTable::getAttributes()
{
    return attributes;
}
map<string, int> AUXCreateTable::getAttributesLength()
{
    return attributes_length;
}
vector<string> AUXCreateTable::getAttributesOrder()
{
    return attributes_order;
}
string AUXCreateTable::getPrimaryKey()
{
    return primary_key;
}
vector<string> AUXCreateTable:: getUniqueKeys()
{
    return unique;
}
//-------------drop table------------

//-------------create index----------
void AUXCreateIndex::setIndexName(string name)
{
    index_name = name;
}
void AUXCreateIndex::setAttributeName(string name)
{
    attribute_name = name;
}
string AUXCreateIndex::getINdexName()
{
    return index_name;
}
string AUXCreateIndex::getAttributeName()
{
    return attribute_name;
}

//------------drop index--------------
void AUXDropIndex::setIndexName(string name)
{
    index_name = name;
}
string AUXDropIndex::getIndexName()
{
    return index_name;
}
//------------insert into-------------
void AUXInsertInto::setAttributes(string attribute)
{
    attributes_name.push_back(attribute);
}
void AUXInsertInto::setValues(string value)
{
    values.push_back(value);
}
vector<string> AUXInsertInto::getAttributeNames()
{
    return attributes_name;
}
vector<string> AUXInsertInto::getValues()
{
    return values;
}

//------------select from-------------
void AUXSelectFrom::setAttributes(string attribute)
{
    attributes.push_back(attribute);
}
void AUXSelectFrom::setConditions(Where condition)
{
    conditions.push_back(condition);
}
vector<string> AUXSelectFrom::getAttributes()
{
    return attributes;
}
vector<Where> AUXSelectFrom::getConditions()
{
    return conditions;
}
void AUXSelectFrom::setIsAllValues(bool in)
{
    all_values = in;
}
void AUXSelectFrom::setIsAllAttributes(bool in)
{
    all_attributes = in;
}
bool AUXSelectFrom::getIsAllValues()
{
    return all_values;
}
bool AUXSelectFrom::getIsAllAttributes()
{
    return all_attributes;
}
//----------delete from-------------
void AUXDeleteFrom::setConditions(Where condition)
{
    conditions.push_back(condition);
}
vector<Where> AUXDeleteFrom::getConditions()
{
    return conditions;
}
void AUXDeleteFrom::setIsDeleteAll(bool in)
{
    delete_all = in;
}
bool AUXDeleteFrom::getIsDeleteAll()
{
    return delete_all;
}
//--------some aux functions--------
bool isNumber(string input)
{
    int countPoint = 0;
    for(int i = 0; i < input.length(); i++)
    {
        if(!((input[i] >= '0' && input[i] <= '9')|| input[i] == '.'))
            return false;
        if(input[i] == '.')
            countPoint++;
    }
    if(countPoint > 1)
        return false;
    return true;
}
bool isFloat(string input)
{
    if(isNumber(input) == false)
        return false;
    for(int i = 0; i < input.length(); i++)
    {
        if(input[i] == '.')
            return true;
    }
    return false;
}
bool isInt(string input)
{
    if (isNumber(input) == false)
        return false;
    for(int i = 0; i < input.length(); i++)
    {
        if(input[i] == '.')
            return false;
    }
    return true;
}
bool checkAttrNameValid(string input)
{
    for(int i = 0; i < input.length(); i++)
    {
        if(!((input[i] >= 'a'&&input[i] <= 'z')||(input[i] >= 'A'&&input[i] <= 'Z')||(input[i] >= '0'&&input[i] <= '9')||input[i] == '_'))
            return false;
    }
    return true;
}

