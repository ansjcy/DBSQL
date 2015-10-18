//
//  interpreter.cpp
//  MiniSQL
//
//  Created by Jason Young on 10/17/15.
//  Copyright © 2015 jason. All rights reserved.
//

#include "interpreter.hpp"
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
    else if(sentence->getType() == "select")
        return dealSelectFrom(sentence);
    else
        return sentence;
}
//read the input, and return an AUX contains the type, as well as the content...
AUX* Interpreter::getInput()
{
    string command(""), command2(""), content("");
    cin>>command;
    AUX *sentence = new AUX();
    //quit SQL
    if(command == "quit;"||command =="quit")
    {
        exit(0);
    }
    //the first word cannot be the end..
    if(command.back()==';')
    {
        sentence->setType("not valid");
        return sentence;
    }
    if(command == "select")
    {
        sentence->setType("select");
    }
    else if(command == "create"||command == "drop"||command == "insert"||command == "delete")
    {
        cin>>command2;
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
        cin>>contentTmp;
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
 test: create table haha ( dep char ( 20 ) , primary key ( dep ) , unique ( dep ) ) ;
*/
AUXCreateTable* Interpreter::dealCreateTable(AUX* sentence)
{
    AUXCreateTable* create_table_packing = new AUXCreateTable();
    stringstream s(sentence->getContent());
    string word, word2, word3;
    bool breakable = false;
    int length = 0;
    
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
        //check the validation of the attr
        if(checkAttrNameValid(word) == false)
        {
            create_table_packing->setType("not valid");
            return create_table_packing;
        }
        //end setting the attributes
        if(word == ";"&&breakable == true)
            break;
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
                    return create_table_packing;
                }
                s>>length;
                s>>word3;
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
    
    create_table_packing->setType("create table");
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
    insert_into_packing->setType("insert into");
    return insert_into_packing;
}

AUXDeleteFrom* Interpreter::dealDeleteFrom(AUX* sentence)
{
    AUXDeleteFrom* delete_from_packing = new AUXDeleteFrom();
    
    return delete_from_packing;
}


//select 2. xx , xx  3.from 4.table name 5.where 6.(where clause);
//test: select id , name from instructor where instructor.id  = 123 ;
AUXSelectFrom* Interpreter::dealSelectFrom(AUX* sentence)
{
    AUXSelectFrom* select_from_packing = new AUXSelectFrom();
    stringstream s(sentence->getContent());
    string word, word2, word3;
    bool breakable = false;
    
    s>>word;
    select_from_packing->setTableName(word);
    s>>word;
    if(word != "(")
    {
        select_from_packing->setType("not valid");
        return select_from_packing;
    }
    while (true) {
        s>>word;
        s>>word2;
        //check the validation of the attr
        if(checkAttrNameValid(word) == false)
        {
            select_from_packing->setType("not valid");
            return select_from_packing;
        }
        //end setting the attributes
        if(word == ";"&&breakable == true)
            break;
        //setting the primary key
        else if(word == "primary" && word2 == "key")
        {
            s>>word;
            if(word!="(")
            {
                select_from_packing->setType("not valid");
                return select_from_packing;
            }
            s>>word;
            s>>word2;
            if(word2 != ")")
            {
                select_from_packing->setType("not valid");
                return select_from_packing;
            }
            select_from_packing->setPrimaryKeys(word);
        }
        //setting unique
        //unique attr can be multiple
        else if(word == "unique" && word2 == "(")
        {
            while (true) {
                s>>word;
                s>>word2;
                select_from_packing->setUnique(word);
                if(word2 == ",")
                    continue;
                else if(word2 == ")")
                    break;
                else{
                    select_from_packing->setType("not valid");
                    return select_from_packing;
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
                    select_from_packing->setType("not valid");
                    return select_from_packing;
                }
                s>>length;
                s>>word3;
                if(word3 != ")")
                {
                    select_from_packing->setType("not valid");
                    return select_from_packing;
                }
            }
            else if(word2 == "int")
            {
                length = sizeof(int);
            }
            else{
                select_from_packing->setType("not valid");
                return select_from_packing;
            }
            select_from_packing->setAttributes(word, word2, length);
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
            select_from_packing->setType("not valid");
            return select_from_packing;
        }
    }
    
    select_from_packing->setType("create table");
    return select_from_packing;
}