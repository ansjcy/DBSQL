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
//------------drop index--------------
void AUXDropIndex::setIndexName(string name)
{
    index_name = name;
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
//------------select from-------------
void AUXSelectFrom::setAttributes(string attribute)
{
    attributes.push_back(attribute);
}
void AUXSelectFrom::setConditions(where condition)
{
    conditions.push_back(condition);
}
//----------delete from-------------
void AUXDeleteFrom::setConditions(where condition)
{
    conditions.push_back(condition);
}

//--------some aux functions--------
bool isNumber(string input)
{
    int countPoint;
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

