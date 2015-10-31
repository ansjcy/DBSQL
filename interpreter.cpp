//  interpreter.cpp
//  MiniSQL
//
//  Created by Jason Young on 10/17/15.
//  Copyright © 2015 jason. All rights reserved.
//

#include "interpreter.hpp"
#include "recorder.hpp"
extern recorder rm;
//read word: 1.create table 2.XX ( 3. name type (size), , , );
//           1.drop table 2.xx ;
//           1.create index 2.index namexx 3. on  4.table name  (5. attr namexx);
//           1.drop index  2.index name  3. on 4. table name;
//           1.insert into 2.xx  3.attr  4.values 5. (the values);
//           1.select 2. xx  3.from 4.table name 5.where 6.(where clause);
//           1.delete from 2.table name  3.where 4.(where clause);

//the start function of interpreter.
AUX* Interpreter::dealInput()
{
    AUX* sentence = getInput();
    if(sentence->getType() == "create table")
        return dealCreateTable(sentence);
    else if(sentence->getType() == "drop table")
        return dealDropTable(sentence);//static_cast<AUX*>(dealDropTable(sentence));
    else if(sentence->getType() == "create index")
        return dealCreateIndex(sentence);
    else if(sentence->getType() == "drop index")
        return dealDropIndex(sentence);
    else if(sentence->getType() == "insert into")
        return dealInsertInto(sentence);
    else if(sentence->getType() == "delete from")
        return dealDeleteFrom(sentence);
    else if(sentence->getType() == "select from")
        return dealSelectFrom(sentence);
    else
        return sentence;
}


//read the input, and return an AUX contains the type, as well as the content...
AUX* Interpreter::getInput()
{
    string norm;
    getline(cin, norm);
    norm = normalize(norm);
    //"create table haha ( dep char ( 20 ) , primary key ( dep ) , unique ( dep ) ) ; "	
    stringstream sin(norm);
    string command(""), command2(""), content("");
    sin>>command;
    AUX *sentence = new AUX();
    //quit SQL
    if(command == "quit;"||command =="quit"){
        sentence->quit = true;
    }
    else
    {
        sentence->quit = false;
    }
    //the first word cannot be the end..
    if(command.back()==';')
    {
        sentence->setType("not valid");
        return sentence;
    }
    if(command == "select")
    {
        sentence->setType("select from");
    }
    else if(command == "create"||command == "drop"||command == "insert"||command == "delete")
    {
        sin>>command2;
        if(command == "create"||command == "drop"){
            if(command2 != "table" && command2!="index")
            {
                sentence->setType("not valid");
                return sentence;
            }
        }
        else if(command == "insert"&&command2 != "into")
        {
            sentence->setType("not valid");
            return sentence;
        }
        else if (command == "delete"&&command2 != "from")
        {
            sentence->setType("not valid");
            return sentence;
        }
        sentence->setType(command + " " +command2);
    }
    //not a command
    else{
        sentence->setType("not valid");
        return sentence;
    }
    string contentTmp;
    while (true) {
        sin>>contentTmp;
        content = content + " " + contentTmp;
        if(contentTmp == ";")
        {
            break;
        }
    }
    sentence->setContent(content);
    return sentence;
}
/*
 content: XX ( 3. name type (size), , , );
 test: create table haha ( dep char ( 20 ) , age int, aggge float(10), primary key ( dep ) , unique ( dep ) ) ;
*/


AUXCreateTable* Interpreter::dealCreateTable(AUX* sentence)
{
    AUXCreateTable* create_table_packing = new AUXCreateTable();
    stringstream s(sentence->getContent());
    string word, word2, word3;
    bool breakable = false;
    int length = 0;
    int float_size = 0;
    
    s>>word;
    create_table_packing->setTableName(word);
    s>>word;
    if(word != "(")
    {
        create_table_packing->setType("not valid");
        return create_table_packing;
    }
    while (true) {
        s>>word;
        s>>word2;
        //end setting the attributes
        if(word == ";"&&breakable == true)
            break;

        //check the validation of the attr
        if(checkAttrNameValid(word) == false)
        {
            create_table_packing->setType("not valid");
            return create_table_packing;
        }
        //setting the primary key
        else if(word == "primary" && word2 == "key")
        {
            s>>word;
            if(word!="(")
            {
                create_table_packing->setType("not valid");
                return create_table_packing;
            }
            s>>word;
            s>>word2;
            if(word2 != ")")
            {
                create_table_packing->setType("not valid");
                return create_table_packing;
            }
            
            create_table_packing->setPrimaryKeys(word);
        }
        //setting unique
        //unique attr can be multiple
        else if(word == "unique" && word2 == "(")
        {
            while (true) {
                s>>word;
                s>>word2;
                create_table_packing->setUnique(word);
                if(word2 == ",")
                    continue;
                else if(word2 == ")")
                    break;
                else{
                    create_table_packing->setType("not valid");
                    return create_table_packing;
                }
            }
        }
        //input the attriutes..
        else{
            if(word2 == "char" || word2 == "float")
            {
                s>>word3;
                if(word3 != "(")
                {
                    create_table_packing->setType("not valid");
                    create_table_packing->setInformation("there should be ( after"+word2);
                    return create_table_packing;
                }
                ////
                if(word2 == "char"){
                    s>>length;
                }
                else
                {
                    s>>float_size;
                }
                ////
                s>>word3;
                if(word2 == "char" && !(length > 0 && length < 256))
                {
                    create_table_packing->setType("not valid");
                    create_table_packing->setInformation("the length of"+word+"is wrong");
                    return create_table_packing;
                }
                if(word2 == "float")
                {
                    length = sizeof(float);
                    create_table_packing->setFloatSize(word, float_size);
                }
                if(word3 != ")")
                {
                    create_table_packing->setType("not valid");
                    return create_table_packing;
                }
            }
            else if(word2 == "int")
            {
                length = sizeof(int);
            }
            else{
                create_table_packing->setType("not valid");
                create_table_packing->setInformation("the type of "+word+" is not correct!");
                return create_table_packing;
            }
            create_table_packing->setAttributes(word, word2, length);
        }
        
        s>>word3;
        //save the attribute
        if(word3 == "," || word3 == ")")
        {
            if(word3 == ")")
                breakable = true;
            continue;
        }
        else
        {
            create_table_packing->setType("not valid");
            return create_table_packing;
        }
    }
    
    vector<string> attrs = create_table_packing->getAttributesOrder();
    vector<string> uniques = create_table_packing->getUniqueKeys();
    int i = 0, j = 0;
    //check whether the unique keys are in the attrs..
    if(uniques.size()!=0)
    for (i = 0; i < uniques.size(); i++) {
        for(j = 0; j < attrs.size(); j++)
        {
            if(attrs[i] == uniques[j])
                break;
        }
        if(j == attrs.size())
        {
            create_table_packing->setType("not valid");
            return create_table_packing;
        }
    }
    
    //check whether the primary key is in the attrs.
    for (i = 0; i < attrs.size(); i++) {
        if(attrs[i] == create_table_packing->getPrimaryKey())
        {
            create_table_packing->setType("create table");
            return create_table_packing;
        }
    }
    create_table_packing->setType("not valid");
    return create_table_packing;
}



// test : drop table haha ;
AUXDropTable* Interpreter::dealDropTable(AUX* sentence)
{
    AUXDropTable* drop_table_packing = new AUXDropTable();
    stringstream s(sentence->getContent());
    string word;
    
    s>>word;
    drop_table_packing->setTableName(word);
    s>>word;
    
    if(word!=";")
    {
        drop_table_packing->setType("not valid");
        return drop_table_packing;
    }
    
    drop_table_packing->setType("drop table");
    return drop_table_packing;
}



//index namexx 3. on  4.table name  (5. attr namexx);
//test: create index haah on yes ( attr ) ;
AUXCreateIndex* Interpreter::dealCreateIndex(AUX* sentence)
{
    AUXCreateIndex* create_index_packing = new AUXCreateIndex();
    stringstream s(sentence->getContent());
    string word, word2;
    
    s>>word;
    create_index_packing->setIndexName(word);
    s>>word;
    if(word != "on")
    {
        create_index_packing->setType("not valid");
        return create_index_packing;
    }
    s>>word;
    create_index_packing->setTableName(word);
    s>>word2;
    if(word2 != "(")
    {
        create_index_packing->setType("not valid");
        return create_index_packing;
    }
    s>>word;
    s>>word2;
    if(word2 != ")")
    {
        create_index_packing->setType("not valid");
        return create_index_packing;
    }
    s>>word2;
    if(word2 != ";")
    {
        create_index_packing->setType("not valid");
        return create_index_packing;
    }
    create_index_packing->setAttributeName(word);
    create_index_packing->setType("create index");

    return create_index_packing;
}



//index name  3. on 4. table name;
//test : drop index haha on aa ;
AUXDropIndex* Interpreter::dealDropIndex(AUX* sentence)
{
    AUXDropIndex* drop_index_packing = new AUXDropIndex();
    stringstream s(sentence->getContent());
    string word, word2;
    
    s>>word;
    drop_index_packing->setIndexName(word);
    s>>word;
    
    if(word != "on")
    {
        drop_index_packing->setType("not valid");
        return drop_index_packing;
    }
    s>>word;
    s>>word2;
    if(word2!=";")
    {
        drop_index_packing->setType("not valid");
        return drop_index_packing;
    }
    drop_index_packing->setTableName(word);
    
    drop_index_packing->setType("drop index");
    return drop_index_packing;
}



//2.tableName  3.attr  4.values 5. (the values);
//test: insert into haha values ( ' qq ' , ' pp ' ) ;
//      insert into haha ( id , sex ) values ( ' qq ' , ' pp ' ) ;
AUXInsertInto* Interpreter::dealInsertInto(AUX* sentence)
{
    AUXInsertInto* insert_into_packing = new AUXInsertInto();
    stringstream s(sentence->getContent());
    string word, word2, word3;
    bool breakable = false;
    
    s>>word;
    insert_into_packing->setTableName(word);
    s>>word2;
    //the user appoints the attributes.
    if(word2 == "(")
    {
        while (true) {
            s>>word;
            s>>word2;
            if(word2 != ")" && word2 != ",")
            {
                insert_into_packing->setType("not valid");
                return insert_into_packing;
            }
            insert_into_packing->setAttributes(word);
            if(word2 == ")")
                break;
            else if(word2 == ",")
                continue;
        }
        s>>word2;
    }
    //the user doesn't appoint the attribute, this is the values.
    if(word2 == "values")
    {
        s>>word2;
        if(word2 != "(")
        {
            insert_into_packing->setType("not valid");
            return insert_into_packing;
        }
        while (true) {
            s>>word;
            if(word == ";"&&breakable == true)
                break;
            //read the string, which can contain some extra symbols, such as ' ' ',' ';'
            if(word == "'"){
                s>>word;
                s>>word2;
                while(word2!="'"){
                    word = word+" "+word2;
                    s>>word2;
                    if(word2 == "")
                    {
                        insert_into_packing->setType("not valid");
                        return insert_into_packing;
                    }
                }
                insert_into_packing->setValues(word);
            }
            //if is a number, float or int..
            else if(isNumber(word) == true)
            {
                insert_into_packing->setValues(word);
            }
            //neither float nor int
            else{
                insert_into_packing->setType("not valid");
                return insert_into_packing;
            }
            s>>word2;
            if(word2 == "," || word2 == ")")
            {
                if(word2 == ")")
                    breakable = true;
                continue;
            }
            else{
                insert_into_packing->setType("not valid");
                return insert_into_packing;
            }
            
        }
    }
    else{
        insert_into_packing->setType("not valid");
        return insert_into_packing;
    }
    insert_into_packing->setType("insert into");
    return insert_into_packing;
}



//delete from 2.table name  3.where 4.(where clause);
//test: delete from instructor where id  = 123 and name = ' ha ha ' ;
//      delete from instructor ;
AUXDeleteFrom* Interpreter::dealDeleteFrom(AUX* sentence)
{
    AUXDeleteFrom* delete_from_packing = new AUXDeleteFrom();
    stringstream s(sentence->getContent());
    string word, word2, word3;
    
    s>>word;
    //now only one table!!
    delete_from_packing->setTableName(word);
    s>>word;
    //select all attributes;
    if(word == ";")
    {
        delete_from_packing->setIsDeleteAll(true);
    }
    else if(word!="where")
    {
        delete_from_packing->setType("not valid");
        return delete_from_packing;
    }
    //deal with where sentence..word == where
    else
    {
        delete_from_packing->setIsDeleteAll(false);
        Where whereTmp;
        while (true) {
            s>>word;
            s>>word2;
            if(checkAttrNameValid(word) == false)
            {
                delete_from_packing->setType("not valid");
                return delete_from_packing;
            }
            whereTmp.attribute = word;
            if(!(word2 == "<"||word2 == "<="||word2 == ">"||word2 == ">="||word2 == "="||word2 == "!="))
            {
                delete_from_packing->setType("not valid");
                return delete_from_packing;
            }
            whereTmp.op = word2;
            
            s>>word;
            if(word == "'"){
                s>>word;
                s>>word2;
                while(word2!="'"){
                    word = word+" "+word2;
                    s>>word2;
                    if(word2 == "")
                    {
                        delete_from_packing->setType("not valid");
                        return delete_from_packing;
                    }
                }
                whereTmp.parameter = word;
            }
            //if is a number, float or int..
            else if(isNumber(word) == true)
            {
                whereTmp.parameter = word;
            }
            //neither float nor int
            else{
                delete_from_packing->setType("not valid");
                return delete_from_packing;
            }
            delete_from_packing->setConditions(whereTmp);
            
            s>>word;
            if(word == ";")
                break;
            else if(word == "and")
            {
                continue;
            }
            else{
                delete_from_packing->setType("not valid");
                return delete_from_packing;
            }
        }
    }
    
    delete_from_packing->setType("delete from");
    return delete_from_packing;
}



//select 2. xx , xx  3.from 4.table name 5.where 6.(where clause);
//test: select id , name from instructor where id  = 123 and name = ' ha ha ' ;
//      select * from instructor where id  = 123 ;
//      select id , name from instructor ;
//      select * from instructor ;
AUXSelectFrom* Interpreter::dealSelectFrom(AUX* sentence)
{
    AUXSelectFrom* select_from_packing = new AUXSelectFrom();
    stringstream s(sentence->getContent());
    string word, word2, word3;
    
    s>>word;
    //select all attributes
    if(word == "*")
    {
        s>>word2;
        if(word2 != "from")
        {
            select_from_packing->setType("not valid");
            return select_from_packing;
        }
        select_from_packing->setIsAllAttributes(true);
    }
    else
    {
        select_from_packing->setIsAllAttributes(false);
        while (true) {
            select_from_packing->setAttributes(word);
            s>>word2;
            if(word2 == "from")
                break;
            if(word2 != ",")
            {
                select_from_packing->setType("not valid");
                return select_from_packing;
            }
            s>>word;
        }
    }
    //now word2 == from
    s>>word;
    //now only one table!!
    select_from_packing->setTableName(word);
    s>>word;
    //select all attributes;
    if(word == ";")
    {
        select_from_packing->setIsAllValues(true);
    }
    else if(word!="where")
    {
        select_from_packing->setType("not valid");
        return select_from_packing;
    }
    //deal with where sentence..word == where
    else
    {
        select_from_packing->setIsAllValues(false);
        Where whereTmp;
        while (true) {
            s>>word;
            s>>word2;
            if(checkAttrNameValid(word) == false)
            {
                select_from_packing->setType("not valid");
                return select_from_packing;
            }
            whereTmp.attribute = word;
            if(!(word2 == "<"||word2 == "<="||word2 == ">"||word2 == ">="||word2 == "="||word2 == "!="))
            {
                select_from_packing->setType("not valid");
                return select_from_packing;
            }
            whereTmp.op = word2;
            
            s>>word;
            if(word == "'"){
                s>>word;
                s>>word2;
                while(word2!="'"){
                    word = word+" "+word2;
                    s>>word2;
                    if(word2 == "")
                    {
                        select_from_packing->setType("not valid");
                        return select_from_packing;
                    }
                }
                whereTmp.parameter = word;
            }
            //if is a number, float or int..
            else if(isNumber(word) == true)
            {
                whereTmp.parameter = word;
            }
            //neither float nor int
            else{
                select_from_packing->setType("not valid");
                return select_from_packing;
            }
            select_from_packing->setConditions(whereTmp);
            
            s>>word;
            if(word == ";")
                break;
            else if(word == "and")
            {
                continue;
            }
            else{
                select_from_packing->setType("not valid");
                return select_from_packing;
            }
        }
    }
    
    select_from_packing->setType("select from");
    return select_from_packing;
}