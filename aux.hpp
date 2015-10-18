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
class AUX
{
protected:
    string database_name;
    string table_name;
    string type;
    string content;
    
public:
    AUX(){}
    void setTableName(string name){table_name = name;}
    void setType(string type_in){type = type_in;}
    void setContent(string content_in){content = content_in;}
    string getType(){return type;}
    string getDatabaseName(){return database_name;}
    string getTableName(){return table_name;}
    string getContent(){return content;}
};
class where
{
public:
    string attribute;
    string op;
    string parameter;
};
class AUXCreateTable : public AUX
{
private:
    //3 variables for attr
    map<string, string> attributes;
    map<string, int> attributes_length;
    vector<string> attributes_order;
    
    string primary_key;
    vector<string> unique;
public:
    AUXCreateTable(){}
    void setAttributes(string attr_name, string attr_type, int length);
    void setPrimaryKeys(string key);
    void setUnique(string unique);

};

class AUXDropTable : public AUX
{
private:
    int useless;
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
};

class AUXDropIndex : public AUX
{
private:
    string index_name;
public:
    void setIndexName(string name);
};

class AUXInsertInto : public AUX
{
private:
    vector<string> attributes_name;
    vector<string> values;
public:
    void setAttributes(string value);
    void setValues(string value);
};

class AUXSelectFrom : public AUX
{
private:
    vector<string> attributes;
    vector<where> conditions;
public:
    void setAttributes(string attribute);
    void setConditions(where condition);
};

class AUXDeleteFrom : public AUX
{
private:
    vector<where> conditions;
public:
    void setConditions(where condition);
};

bool isNumber(string input);
bool isFloat(string input);
bool isInt(string input);
bool checkAttrNameValid(string input);


#endif /* aux_hpp */
