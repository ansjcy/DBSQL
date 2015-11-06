//
//  interpreter.hpp
//  MiniSQL
//
//  Created by jason on 10/17/15.
//  Copyright © 2015 jason. All rights reserved.
//

#ifndef interpreter_hpp
#define interpreter_hpp
#include "auxiliary.hpp"
#include <iostream>
#include <sstream>
using namespace std;
//read word: 1.create table 2.XX ( 3. name type (size), , , )
//           1.drop table 2.xx
//           1.create index 2.index namexx 3. on  4.table name  (5. attr namexx)
//           1.drop index  2.index name  3. on 4. table name
//           1.insert into 2.xx  3.attr  4.values 5. (the values)
//           1.select 2. xx  3.from 4.table name 5.where 6.(where clause)
//           1.delete from 2.table name  3.where 4.(where clause)
class Interpreter
{
public:
    
    AUX* dealInput();
    void dealInput(stringstream input);
    AUX* getInput();
    AUX* getInputExec();
    AUXCreateTable* dealCreateTable(AUX* sentence);
    AUXDropTable* dealDropTable(AUX* sentence);
    AUXCreateIndex* dealCreateIndex(AUX* sentence);
    AUXDropIndex* dealDropIndex(AUX* sentence);
    AUXInsertInto* dealInsertInto(AUX* sentence);
    AUXSelectFrom* dealSelectFrom(AUX* sentence);
    AUXDeleteFrom* dealDeleteFrom(AUX* sentence);
};
#endif /* interpreter_hpp */
