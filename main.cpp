//
//  main.cpp
//  MiniSQL
//
//  Created by jason on 10/9/15.
//  Copyright © 2015 jason. All rights reserved.
//

#include <iostream>
#include <string>
#include <sstream>
#include <set>
#include "auxiliary.hpp"
#include "interpreter.hpp"
#include "api.hpp"
using namespace std;
int main(void){
    Interpreter interpreter;
    //AUXInsertInto* newwww = static_cast<AUXInsertInto*>(neww);
    API api;
    while (true) {
        AUX* neww =  interpreter.dealInput();
        if(neww->quit)
            return 0;
        api.chooseCommand(neww);
    }
        cout<<"haha";
}

//create table haha ( dep char ( 20 ) , depp int , primary key ( dep ) ) ;
//insert into haha(id,sex)values( 'qq ', 'pp ');
//create table haha ( dep char ( 20 ) , age int, aggge float(10), primary key ( dep ) , unique ( dep ) ) ;
//insert into haha values ('abcde', 1, 1.4);
//insert into haha values('fuck', 123, 123.33);
//insert into haha values('zhzhang', 1, 2.22);
//select * from haha where dep='fuck' and age = 123;
//select * from haha;
//delete from haha;