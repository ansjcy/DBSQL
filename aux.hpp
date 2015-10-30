//
//  aux.hpp
//  MiniSQL
//
//  Created by jason on 10/11/15.
//  Copyright © 2015 jason. All rights reserved.
//

#ifndef aux_hpp
#define aux_hpp

#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;
/*
    create table
    drop table
    create index
    drop index
    insert into
    select from
    delete from
 
 create table student ( 
    sno char(8),
    sname char(16) unique, 
    sage int,
    sgender char (1), 
    primary key ( sno )
 );
 drop table student;
 create index stunameidx on student ( sname );
 DROP INDEX indexname ON tblname
 select * from student where sage > 20 and sgender = ‘F’;
 delete from student where sage > 20 and sgender = ‘F’
 */
class Where
{
public:
    string attribute;
    string op;
    string parameter;
};
class WhereForRecorder
{
public:
    int type;
    string op;
    string value;
    int offset; //the first: 0
    int length; //int, float: 0,  char: not 0
};

class AUX
{
protected:
    string database_name;
    string table_name;
    string type;
    string information;
    string content;
    
public:
    AUX(){}
    void setTableName(string name){table_name = name;}
    void setType(string type_in){type = type_in;}
    void setContent(string content_in){content = content_in;}
    void setInformation(string info){information = info;}
    string getType(){return type;}
    string getInformation(){return information;}
    string getDatabaseName(){return database_name;}
    string getTableName(){return table_name;}
    string getContent(){return content;}
};

class AUXCreateTable : public AUX
{
private:
    //3 variables for attr
    map<string, string> attributes;
    map<string, int> attributes_length;
    map<string, int> float_size;
    vector<string> attributes_order;
    
    string primary_key;
    vector<string> unique;
public:
    AUXCreateTable(){}
    void setAttributes(string attr_name, string attr_type, int length);
    void setFloatSize(string attr_name, int size);
    void setPrimaryKeys(string key);
    void setUnique(string unique);
    map<string, string> getAttributes();
    map<string, int> getAttributesLength();
    vector<string> getAttributesOrder();
    string getPrimaryKey();
    vector<string> getUniqueKeys();
};

class AUXDropTable : public AUX
{
public:
    AUXDropTable(){}
};

class AUXCreateIndex : public AUX
{
private:
    string index_name;
    string attribute_name;
public:
    void setIndexName(string name);
    void setAttributeName(string name);
    string getIndexName();
    string getAttributeName();
};

class AUXDropIndex : public AUX
{
private:
    string index_name;
public:
    void setIndexName(string name);
    string getIndexName();
};

class AUXInsertInto : public AUX
{
private:
    vector<string> attributes_name;
    vector<string> values;
public:
    void setAttributes(string value);
    void setValues(string value);
    vector<string> getAttributeNames();
    vector<string> getValues();
};

class AUXSelectFrom : public AUX
{
private:
    vector<string> attributes;
    vector<Where> conditions;
    bool all_values = false;
    bool all_attributes = false;
public:
    void setAttributes(string attribute);
    void setConditions(Where condition);
    void setIsAllValues(bool in);
    void setIsAllAttributes(bool in);
    vector<string> getAttributes();
    vector<Where> getConditions();
    bool getIsAllValues();
    bool getIsAllAttributes();
};

class AUXDeleteFrom : public AUX
{
private:
    vector<Where> conditions;
    bool delete_all = false;
public:
    void setConditions(Where condition);
    vector<Where> getConditions();
    void setIsDeleteAll(bool in);
    bool getIsDeleteAll();
};

bool isNumber(string input);
bool isFloat(string input);
bool isInt(string input);
bool checkAttrNameValid(string input);
string normalize(string input);


#endif /* aux_hpp */
