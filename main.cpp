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
#include <fstream>
using namespace std;
int main(void){
    vector<int>b;
    set<int> a;
    b.push_back(1);
    b.push_back(2);
    b.push_back(1);
    b.push_back(2);
    a.insert(b.begin(), b.end());
    Interpreter interpreter;
    //AUXInsertInto* newwww = static_cast<AUXInsertInto*>(neww);
    API api;
    fstream fp;
    //fp.open("/Users/jason/Desktop/file", ios::in|ios::binary);
    //freopen("/Users/jason/Desktop/file", "r", stdin);
    while (true) {
        AUX* neww =  interpreter.dealInput();
        if(neww->quit)
            return 0;
        api.chooseCommand(neww);
    }
            cout<<"haha";
}


//create table haha ( dep char ( 20 ) , age int, money float(10), primary key ( dep ) , unique ( age ) ) ;
//insert into haha values ('abcde', 1, 1.4);
//insert into haha values('fuck', 123, 123.33);
//insert into haha values('zhzhang', 1, 2.22);
//select * from haha where dep='fuck' and age = 123;
//select * from haha;
//delete from haha;
//create index iii on haha ( dep ) ;
//drop table haha;
//create index iii on haha (age) ;
//drop index haha on aa ;