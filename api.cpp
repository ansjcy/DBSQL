//
//  api.cpp
//  MiniSQL
//
//  Created by jason on 10/18/15.
//  Copyright © 2015 jason. All rights reserved.
//

#include "api.hpp"
void API::chooseCommand(AUX* command)
{
    string type = command->getType();
    if(type == "not valid")
    {
        isSucceed = false;
        information = "there is an error in the command!";
    }
    else if(type == "create table")
    {
        AUXCreateTable* cast_command = static_cast<AUXCreateTable*>(command);
        createTable(cast_command);
    }
    else if(type == "drop table")
    {
        AUXDropTable* cast_command = static_cast<AUXDropTable*>(command);
        dropTable(cast_command);
    }
    else if(type == "create index")
    {
        AUXCreateIndex* cast_command = static_cast<AUXCreateIndex*>(command);
        createIndex(cast_command);
    }
    else if(type == "drop index")
    {
        AUXDropIndex* cast_command = static_cast<AUXDropIndex*>(command);
        dropIndex(cast_command);
    }
    else if(type == "insert into")
    {
        AUXInsertInto* cast_command = static_cast<AUXInsertInto*>(command);
        InsertInto(cast_command);
    }
    else if(type == "delete from")
    {
        AUXDeleteFrom* cast_command = static_cast<AUXDeleteFrom*>(command);
        deleteFrom(cast_command);
    }
    else if(type == "select from")
    {
        AUXSelectFrom* cast_command = static_cast<AUXSelectFrom*>(command);
        selectFrom(cast_command);
    }
    printInformation();
}
void API::printInformation()
{
    if(isSucceed == false)
    {
        cout<<information;
        return;
    }
    else
        cout<<"success";
}
void API::createTable(AUXCreateTable* cast_command)
{
    
}
void API::dropTable(AUXDropTable* cast_command)
{
    //table exists? if not, return
    
    //get the information of this table
        //if has indexes, drop these indexes. if failed, return
    
    //drop table, if success, return. failed, return..
}
void API::createIndex(AUXCreateIndex* cast_command)
{
    //table exists? if not , return
    //index exists? if so, return
    //if has table and index
        //attribute exist? if not, return
        //attribute unique? if not, return
    //create index and add records..
}
void API::dropIndex(AUXDropIndex* cast_command)
{
    
}
void API::InsertInto(AUXInsertInto* cast_command)
{
    
}
void API::selectFrom(AUXSelectFrom* cast_command)
{
    
}
void API::deleteFrom(AUXDeleteFrom* cast_command)
{
    
}
