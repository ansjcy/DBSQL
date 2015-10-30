//
//  api.hpp
//  MiniSQL
//
//  Created by jason on 10/18/15.
//  Copyright © 2015 jason. All rights reserved.
//

#ifndef api_hpp
#define api_hpp
#include "auxiliary.hpp"
#include "interpreter.hpp"
#include "indexManager.hpp"
#include "catalog_manager.h"
#include "recordManager.hpp"
#include "recorder.hpp"
#include <stdio.h>
//           1.create table 2.XX ( 3. name type (size), , , );
//           1.drop table 2.xx ;
//           1.create index 2.index namexx 3. on  4.table name  (5. attr namexx);
//           1.drop index  2.index name  3. on 4. table name;
//           1.insert into 2.xx  3.attr  4.values 5. (the values);
//           1.select 2. xx  3.from 4.table name 5.where 6.(where clause);
//           1.delete from 2.table name  3.where 4.(where clause);
class API{
private:
    bool isSucceed = true;
    //string information;
public:
    void chooseCommand(AUX* command);
    //void setInformation(string info);
    void setSucceed(bool success);
    void printInformation(AUX* command);
    
    void createTable(AUXCreateTable* cast_command);
    void dropTable(AUXDropTable* cast_command);
    void createIndex(AUXCreateIndex* cast_command);
    void dropIndex(AUXDropIndex* cast_command);
    void InsertInto(AUXInsertInto* cast_command);
    void selectFrom(AUXSelectFrom* cast_command);
    void deleteFrom(AUXDeleteFrom* cast_command);
    //toRightDataKind();
};
#endif /* api_hpp */
