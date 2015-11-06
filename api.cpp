//
//  api.cpp
//  MiniSQL
//
//  Created by jason on 10/18/15.
//  Copyright © 2015 jason. All rights reserved.
//

#include "api.hpp"
#include <iomanip>
string file_addr = "/Users/jason/Desktop/";
extern recorder rm;
extern CatalogManager cm;
extern IndexManager im;

void API::chooseCommand(AUX* command)
{
    string type = command->getType();
    if(type == "not valid")
    {
        isSucceed = false;
        //information = "there is an error in the command!";
        cout<<"NOT VALID!!!"<<endl;
    }
    else if(type == "create table")
    {
        AUXCreateTable* cast_command = static_cast<AUXCreateTable*>(command);
        createTable(cast_command);
        cout<<cast_command->getInformation()<<endl;
    }
    else if(type == "drop table")
    {
        AUXDropTable* cast_command = static_cast<AUXDropTable*>(command);
        dropTable(cast_command);
        cout<<cast_command->getInformation()<<endl;
    }
    else if(type == "create index")
    {
        AUXCreateIndex* cast_command = static_cast<AUXCreateIndex*>(command);
        createIndex(cast_command);
        cout<<cast_command->getInformation()<<endl;
    }
    else if(type == "drop index")
    {
        AUXDropIndex* cast_command = static_cast<AUXDropIndex*>(command);
        dropIndex(cast_command);
        cout<<cast_command->getInformation()<<endl;
    }
    else if(type == "insert into")
    {
        AUXInsertInto* cast_command = static_cast<AUXInsertInto*>(command);
        InsertInto(cast_command);
        cout<<cast_command->getInformation()<<endl;
    }
    else if(type == "delete from")
    {
        AUXDeleteFrom* cast_command = static_cast<AUXDeleteFrom*>(command);
        deleteFrom(cast_command);
        cout<<cast_command->getInformation()<<endl;
    }
    else if(type == "select from")
    {
        AUXSelectFrom* cast_command = static_cast<AUXSelectFrom*>(command);
        selectFrom(cast_command);
        cout<<cast_command->getInformation()<<endl;
    }
    
}


//void API::setInformation(string info)
//{
//    information = info;
//}
//void API::printInformation(AUX* command)
//{
//    if(isSucceed == false)
//    {
//        cout<<command->;
//        return;
//    }
//    else
//        cout<<"success";
//}
void API::setSucceed(bool success)
{
    isSucceed = success;
}
/*
 map<string, string> attributes;
 map<string, int> attributes_length;
 vector<string> attributes_order;
 
 string primary_key;
 vector<string> unique;
 */


void API::createTable(AUXCreateTable* cast_command)  //done
{
    TableInfo table_info;
    AttrInfo attr_info;
    string table_name = cast_command->getTableName();
    string primary_key = cast_command->getPrimaryKey();
    vector<string>uniques = cast_command->getUniqueKeys();
    string tmpFileAddr = file_addr + table_name;

    //recorder rm(file_addr);
    
    //@@@@@@@hastable, mind this function..
//    if (cm.hasTable(table_name)){
//        setSucceed(false);
//        cast_command->setInformation("The table has existed!");
//        return;
//    }
    
    table_info.setName(table_name);
    map<string, string>attrs = cast_command->getAttributes();
    map<string, int>attrs_length = cast_command->getAttributesLength();
    map<string, string>::iterator it;
    vector<string> attr_order = cast_command->getAttributesOrder();
    vector<string>::iterator newIt;
    map<string, int>::iterator itLen;
    short element_size = 0;
    string tmpIndexName;
    for (newIt = attr_order.begin() /*it = attrs.begin(),itLen = attrs_length.begin()*/; newIt != attr_order.end();newIt++ /*it++,itLen++*/) {
        it = attrs.find(newIt->data());
        itLen = attrs_length.find(newIt->data());
        
        attr_info.setName(it->first);
        int tmp_type = (it->second == "int")?0:(it->second == "float")?1:(it->second == "char")?2:99;
        attr_info.setTypeId(tmp_type);
        element_size+=itLen->second;
        attr_info.setStrLen(itLen->second);
        attr_info.setHasIndex(false);
        attr_info.setIsPrimaryKey(false);
        attr_info.setIsUnique(false);

        //is primary key
        if(it->first == primary_key)
        {
            attr_info.setIsPrimaryKey(true);
            attr_info.setIsUnique(true);
            tmpIndexName = tmpFileAddr + "_" + primary_key + ".index";
            //$$$create index on this table
            //im.createIndex(tmpIndexName, tmp_type, itLen->second);
            attr_info.setHasIndex(true);
            
        }
        //is unique
        for(int i = 0; i < uniques.size(); i++)
        {
            if(uniques[i] == it->first)
            {
                attr_info.setIsUnique(true);
                break;
            }
        }
        //add attr
        table_info.addAttr(it->first, attr_info);
    }
    cm.createTableInfo(table_info);
    
    im.createIndex(tmpIndexName);
    rm.createTable(tmpFileAddr, element_size);
    
    
}


void API::dropTable(AUXDropTable* cast_command)  //done
{
    TableInfo table_info;
    AttrInfo attr_info;
    string table_name = cast_command->getTableName();
    string tmpFileAddr = file_addr + table_name;
    
    //table exists? if not, return
    if(!cm.hasTable(table_name))
    {
        setSucceed(false);
        cast_command->setInformation("there is no such table!");
        return;
    }
    //get the information of this table
    table_info = cm.getTableInfo(table_name);
    map<string, AttrInfo> attrs = table_info.getAllAttrs();
    vector<string> indexes = table_info.getIndex();
    //if has indexes, drop these indexes. if failed, return
    if(!indexes.empty())
    {
        //@@@@@@drop index!
        for(int i = 0; i < indexes.size(); i++)
        {
            string tmpIndexName = tmpFileAddr + "_" + indexes[i] + ".index";
            im.dropIndex(tmpIndexName);
        }
        
    }
    //@@@@@@drop table, if success, return. failed, return..
    
    cm.dropTableInfo(tmpFileAddr);
    rm.dropTable(tmpFileAddr);
}


void API::createIndex(AUXCreateIndex* cast_command)  //done
{
    TableInfo table_info;
    AttrInfo attr_info;
    string table_name = cast_command->getTableName();
    string index_name = cast_command->getIndexName();
    string attr_name = cast_command->getAttributeName();
    string tmpFileAddr = file_addr + table_name;
    string tmpIndexName = tmpFileAddr + "_" + index_name + ".index";

    //table exists? if not , return
    if(!cm.hasTable(table_name))
    {
        setSucceed(false);
        cast_command->setInformation("there is no such table!");
        return;
    }
    
    table_info = cm.getTableInfo(table_name);
    map<string, AttrInfo> attrs = table_info.getAllAttrs();
    map<string, AttrInfo>::iterator it;
    
    //attribute exist? if not, return
    it = attrs.find(attr_name);
    if(it == attrs.end())
    {
        setSucceed(false);
        cast_command->setInformation("there is no such attr!");
        return;
    }

    //index exists? if so, return
    if(attrs[attr_name].getHasIndex())
    {
        setSucceed(false);
        cast_command->setInformation("this attr has had index!");
        return;
    }
    
    //attribute unique? if not, return
    if(!attrs[attr_name].getIsUnique())
    {
        setSucceed(false);
        cast_command->setInformation("this attr is not unique!");
        return;
    }
    //@@@@@@create index and add records..
    table_info.getAttr(attr_name).setHasIndex(true);
    im.createIndex(tmpIndexName);
    vector<int> AttrAddr;
    AttrType attr_value;
    map<AttrType, int> datalist;
    vector<string> attrNames = table_info.getAttrNames();
    //string& tableName, vector<long>* addr = NULL
    vector<void*> result = rm.selectNoWhere(tmpFileAddr, &AttrAddr);
    int thisType = attrs.find(attr_name)->second.getTypeId();
    
    //the values
    int k = 0;
    for(int j = 0; j < result.size(); j++)
    {
        int size = 0;
        for(int i = 0; i < attrs.size(); i++)
        {
            int type = table_info.getAttr(attrNames[i]).getTypeId();
            if(type == thisType) //int
            {
                //cout<<*(int*)((char*)result[j]+size)<<"\t";
                attr_value = AttrType(*(int*)((char*)result[j]+size));
                datalist[attr_value] = AttrAddr[k++];
                size += 4;
                
            }
            else if(type == thisType) //float
            {
                //cout<<*(float*)((char *)result[j]+size)<<"\t";
                attr_value = AttrType(*(float*)((char*)result[j]+size));
                datalist[attr_value] = AttrAddr[k++];
                size += 4;
            }
            else if(type == thisType)
            {
                int temp = table_info.getAttr(attrNames[i]).getStrLen();
                //cout<<setw(temp) <<(char*)result[j]+size<<"\t";
                
                attr_value = AttrType(*(char*)((char*)result[j]+size));
                datalist[attr_value] = AttrAddr[k];
                k+=temp;

                size += temp;
            }
        }
        cout<<endl;
    }

    
    im.insertIntoIndex(datalist);
    cm.writeTableInfo(table_info);
    
    //needs im.addrecord and cm.addindex..
}


void API::dropIndex(AUXDropIndex* cast_command)
{
    TableInfo table_info;
    AttrInfo attr_info;
    string table_name = cast_command->getTableName();
    string index_name = cast_command->getIndexName();
    string tmpFileAddr = file_addr + table_name;
    string tmpIndexName = tmpFileAddr + "_" + index_name + ".index";
    
    //table exists? if not , return
    if(!cm.hasTable(table_name))
    {
        setSucceed(false);
        cast_command->setInformation("there is no such table!");
        return;
    }
    
    vector<string> indexes = table_info.getIndex();
    //index exists??
    if(indexes.empty())
    {
        setSucceed(false);
        cast_command->setInformation("there is no such index!");
        return;
    }
    int i;
    for(i = 0; i < indexes.size(); i++)
    {
        if(indexes[i] == index_name)
        {
            break;
        }
    }
    if(i == indexes.size())
    {
        setSucceed(false);
        cast_command->setInformation("there is no such index!");
        return;
    }
    //drop index and updata table info..
    im.dropIndex(tmpIndexName);
    for(int i = 0; i < table_info.getAttrNames().size(); i++)
    {
        //if(table_info.getAttr(table_info.getAttrNames()[i]))
    }
    cm.writeTableInfo(table_info);
    //cm??
    //im.dropIndex(tmpFileAddr); ?
}


void API::InsertInto(AUXInsertInto* cast_command)  //finish
{
    TableInfo table_info;
    AttrInfo attr_info;
    string table_name = cast_command->getTableName();
    char dt[4096];
    int bias = 0;
    //table exists? if not , return
    if(!cm.hasTable(table_name))
    {
        setSucceed(false);
        cast_command->setInformation("there is no such table!");
        return;
    }
    
    table_info = cm.getTableInfo(table_name);
    vector<string> attrs = table_info.getAttrNames();
    vector<string> values;
    
    //has input the attrs
    if(!cast_command->getAttributeNames().empty())
    {
        //check is the same length
        if(cast_command->getAttributeNames().size() != cast_command->getValues().size())
        {
            setSucceed(false);
            cast_command->setInformation("the number of values is not the same with the number of attrs");
            return;
        }
        //sort the values
        for(int j = 0; j < attrs.size(); j++)
        {
            int i;
            for(i = 0; i < cast_command->getAttributeNames().size(); i++)
            {
                if(cast_command->getAttributeNames()[i] == attrs[j])
                {
                    values.push_back(cast_command->getValues()[i]);
                    break;
                }
            }
            if(i == cast_command->getAttributeNames().size())
            {
                setSucceed(false);
                cast_command->setInformation("there is no such attr!!");
                return;
            }
        }
    }
    else
    {
        values = cast_command->getValues();
    }
    //check overflow
    for(int i = 0; i < values.size(); i++)
    {
        int type_id = table_info.getAttr(attrs[i]).getTypeId(); //0:int, 1:float, 2:string.
        int str_length = table_info.getAttr(attrs[i]).getStrLen();
        if(type_id == 0) //int
        {
            if(!isInt(values[i]))
            {
                setSucceed(false);
                cast_command->setInformation(values[i]+" it is not a integer");
                return;
            }
            try
            {
                int tmp = stoi(values[i]);
                memcpy(dt+bias, &tmp, sizeof(int));
                bias+=sizeof(int);
            }
            catch(out_of_range error){
                setSucceed(false);
                cast_command->setInformation("integer out of bounds");
                return;
            }
        }
        else if(type_id == 1)  //float
        {
            if(!isFloat(values[i]))
            {
                setSucceed(false);
                cast_command->setInformation("is is not a float number");
                return;
            }
            try
            {
                float tmp = stof(values[i]);
                memcpy(dt+bias, &tmp, sizeof(float));
                bias+=sizeof(float);
            }
            catch(out_of_range error){
                setSucceed(false);
                cast_command->setInformation("float out of bounds");
                return;
            }
        }
        else if(type_id == 2)  //char
        {
            if(values[i].length() > str_length)
            {
                setSucceed(false);
                cast_command->setInformation("char out of bounds");
                return;
            }
            for(int j = 0; values[i].length() < str_length; j++)
            {
                values[i] = values[i]+" ";
            }
            memcpy(dt+bias, values[i].c_str(), str_length);
            bias+=str_length;
        }
        
        
    }
    //check inserting into a unique attr (or primary)
        //use recorder..
    //insert and update index..
    string tmpFileAddr = file_addr + table_name;
    int offset = (int)rm.insert(dt, tmpFileAddr);
    map<AttrType, int> datalist;
    AttrType attr_value;
    for(int i = 0; i < table_info.getAttrNames().size(); i++)
    {
        if(table_info.getAttr(table_info.getAttrNames()[i]).getHasIndex())
        {
            datalist[table_info.getAttrNames()[i]] = offset;
        }
    }
    im.insertIntoIndex(datalist);

    //get the datalist...from recorder..
    //im.insertIntoIndex(datalist);
}


void API::selectFrom(AUXSelectFrom* cast_command)
{
    TableInfo table_info;
    AttrInfo attr_info;
    string table_name = cast_command->getTableName();
    vector<string> attr_selected;
    
    //@@@@@@@hastable, mind this function..
    if (!cm.hasTable(table_name)){
        setSucceed(false);
        cast_command->setInformation("The table has existed!");
        return;
    }
    table_info = cm.getTableInfo(table_name);
    if(cast_command->getIsAllValues())
    {
        attr_selected = table_info.getAttrNames();
    }
    else{
        attr_selected = cast_command->getAttributes();
        //if the attr exists..
        for(int i = 0; i < attr_selected.size(); i++)
        {
            int j;
            for(j = 0; j < table_info.getAttrNames().size(); j++)
            {
                if(attr_selected[i] == table_info.getAttrNames()[j])
                    break;
            }
            if(j == table_info.getAttrNames().size())
            {
                setSucceed(false);
                cast_command->setInformation("there is no such attr..");
                return;
            }
        }
    }
    //check whether the where sentence is valid
    vector<Where> where = cast_command->getConditions();
    for(int i = 0; i < where.size(); i++)
    {
        int j;
        for(int j = 0; j < table_info.getAttrNames().size(); j++)
        {
            if(where[i].attribute == table_info.getAttrNames()[j])
                break;
        }
        if(j == table_info.getAttrNames().size())
        {
            setSucceed(false);
            cast_command->setInformation("there is no such attr in the where sentence");
            return;
        }
    }
    //insert..
    //with some index
        //get index name, get offset, get data..
    //with no index
        //get data
    string tmpFileAddr = file_addr + table_name;
    vector<string> attrs = table_info.getAttrNames();
    
    //has no where statement!
    if(where.size() == 0){
        vector<void*> result = rm.selectNoWhere(tmpFileAddr);
        
        //the attrs
        cout<<"++++++++++++++++++++++++++"<<endl;
        for(int i = 0; i < attrs.size(); i++)
        {
            cout<<table_info.getAttr(attrs[i]).getName()<<"\t"<<"\t";
        }
        cout<<endl;
        cout<<"++++++++++++++++++++++++++"<<endl;
        //the values
        for(int j = 0; j < result.size(); j++)
        {
            int size = 0;
            for(int i = 0; i < attrs.size(); i++)
            {
                int type = table_info.getAttr(attrs[i]).getTypeId();
                if(type == 0) //int
                {
                    cout<<*(int*)((char*)result[j]+size)<<"\t";
                    size += 4;
                
                }
                else if(type == 1) //float
                {
                    cout<<*(float*)((char *)result[j]+size)<<"\t";
                    size += 4;
                }
                else if(type == 2)
                {
                    int temp = table_info.getAttr(attrs[i]).getStrLen();
                    cout<<setw(temp) <<(char*)result[j]+size<<"\t";
                    size += temp;
                }
            }
            cout<<endl;
        }
    }
    //has nor no index..
    else{
        vector<WhereForRecorder>* cond = new vector<WhereForRecorder>;
        //vector<WhereForRecorder>* condNoIndex = new vector<WhereForRecorder>;
        //vector<WhereForRecorder>* condWithIndex = new vector<WhereForRecorder>;
        WhereForRecorder tmp;
        bool index_flag = true;
        vector<void*> result;
        /*
         int type;
         string op;
         string value;
         int offset; //the first: 0
         int length;
         */
        for(int i = 0; i < where.size(); i++)
        {
            string attribute_name = where[i].attribute;
            tmp.type = table_info.getAttr(attribute_name).getTypeId();
            tmp.op = where[i].op;
            tmp.value = where[i].parameter;
            int tmp_offset = 0;
            for(int j = 0; j < table_info.getAttrNames().size(); j++)
            {
                if(table_info.getAttrNames()[j] == where[i].attribute)
                    break;
                int type = table_info.getAttr(table_info.getAttrNames()[j]).getTypeId();
                if(type == 0)
                {
                    tmp_offset+=4;
                }
                else if(type == 1)
                {
                    tmp_offset+=4;
                }
                else
                {
                    tmp_offset+=table_info.getAttr(table_info.getAttrNames()[j]).getStrLen();
                }
            }
            tmp.offset = tmp_offset;
            tmp.length = table_info.getAttr(where[i].attribute).getStrLen();
            if(!table_info.getAttr(attribute_name).getHasIndex())
            {
                //condWithIndex->push_back(tmp);
                index_flag = false;
            }
//            else
//            {
//                condNoIndex->push_back(tmp);
//                index_flag = false;
//            }
            cond->push_back(tmp);
        }
        int element_size = 0;
        for(int i = 0; i < table_info.getAttrNames().size(); i++)
        {
            element_size+=table_info.getAllAttrs()[table_info.getAttrNames()[i]].getStrLen();
        }
//        conditionJudge jdNoIndex(condNoIndex, element_size);
//        conditionJudge jdWithIndex(condWithIndex, element_size);
//        vector<void*> result = rm.selectWhereNoIndex(tmpFileAddr, jdNoIndex);
        if(!index_flag|| where.size() > 1)
        {
            conditionJudge jd(cond, element_size);
            result = rm.selectWhereNoIndex(tmpFileAddr, jd);
        }
        else{
            vector<int> offset;
            //std::vector<int> findLeft(const AttrType &k);
            //std::vector<int> findInIndex(std::vector<AttrType> datalist)
            int type = cond->operator[](0).type;
            vector<AttrType> datalist;
            
            
            AttrType attr_value;
            if(type == 0)
            {
                AttrType attr_value(stoi(cond->operator[](0).value));
            }
            else if(type == 1)
            {
                AttrType attr_value(stof(cond->operator[](0).value));
            }
            else if(type == 2)
            {
                AttrType attr_value(cond->operator[](0).value);
            }
            
            datalist.push_back(attr_value);
            if(cond->operator[](0).op == "=")
            {
                offset = im.findInIndex(datalist);
                conditionJudge jd(cond, element_size);
                result = rm.selectWhereIndex(tmpFileAddr, offset, jd);
                //rm.selectWhereNoIndex(tmpFileAddr, jd);
            }
            else if (cond->operator[](0).op == ">")
            {
                offset = im.btree.findRight(attr_value);
                conditionJudge jd(cond, element_size);
                result = rm.selectWhereIndex(tmpFileAddr, offset, jd);
            }
            else if (cond->operator[](0).op == "<")
            {
                offset = im.btree.findLeft(attr_value);
                conditionJudge jd(cond, element_size);
                result = rm.selectWhereIndex(tmpFileAddr, offset, jd);

            }
            else if (cond->operator[](0).op == ">=")
            {
                offset = im.btree.findRight(attr_value);
                conditionJudge jd(cond, element_size);
                result = rm.selectWhereIndex(tmpFileAddr, offset, jd);

            }
            else if (cond->operator[](0).op == "<=")
            {
                offset = im.btree.findLeft(attr_value);
                conditionJudge jd(cond, element_size);
                result = rm.selectWhereIndex(tmpFileAddr, offset, jd);
            }
            else if (cond->operator[](0).op == "!=")
            {
                conditionJudge jd(cond, element_size);
                result = rm.selectWhereNoIndex(tmpFileAddr, jd);
            }
            
        }
        
        
        //vector<void*> resultWithIndex = rm.selectWhereIndex(tmpFileAddr, offset.., jdWithIndex);
        //do the intersection...
        //cout << "Result Size is: " <<  result.size() << "\n";
        cout<<"++++++++++++++++++++++++++"<<endl;
        for(int i = 0; i < attrs.size(); i++)
        {
            cout<<table_info.getAttr(attrs[i]).getName()<<"\t"<<"\t";
        }
        cout<<endl;
        cout<<"++++++++++++++++++++++++++"<<endl;
        
        
        for(int j = 0; j < result.size(); j++)
        {
            int size = 0;
            for(int i = 0; i < attrs.size(); i++)
            {
                int type = table_info.getAttr(attrs[i]).getTypeId();
                if(type == 0) //int
                {
                    cout<<*(int*)((char*)result[j]+size)<<"\t";
                    size += 4;
                    
                }
                else if(type == 1) //float
                {
                    cout<<*(float*)((char *)result[j]+size)<<"\t";
                    size += 4;
                }
                else if(type == 2)
                {
                    int temp = table_info.getAttr(attrs[i]).getStrLen();
                    cout<<setw(temp) <<(char*)result[j]+size<<"\t";
                    size += temp;
                }
            }
            cout<<endl;
        }

    }
    cout<<"++++++++++++++++++++++++++"<<endl;
    //deal with the same data..
    //print out the table
    
}


void API::deleteFrom(AUXDeleteFrom* cast_command)
{
    TableInfo table_info;
    AttrInfo attr_info;
    string table_name = cast_command->getTableName();
    vector<string> attr_selected;
    
    //@@@@@@@hastable, mind this function..
    if (!cm.hasTable(table_name)){
        setSucceed(false);
        cast_command->setInformation("The table does not exists!");
        return;
    }
    table_info = cm.getTableInfo(table_name);
    
    vector<Where> where = cast_command->getConditions();
    for(int i = 0; i < where.size(); i++)
    {
        int j;
        for(int j = 0; j < table_info.getAttrNames().size(); j++)
        {
            if(where[i].attribute == table_info.getAttrNames()[j])
                break;
        }
        if(j == table_info.getAttrNames().size())
        {
            setSucceed(false);
            cast_command->setInformation("there is no such attr in the where sentence");
            return;
        }
    }
    //do the deleting
    string tmpFileAddr = file_addr + table_name;
    //string tmpIndexName = tmpFileAddr + "_" + index_name + ".index";
    if (cast_command->getIsDeleteAll()) {
        //delete in recorder
        rm.deleteNoWhere(tmpFileAddr);
        //update index
        for (int i = 0; i < table_info.getAttrNames().size(); i++) {
            if(table_info.getAttr(table_info.getAttrNames()[i]).getHasIndex() == true)
            {
                string tmpIndexName = tmpFileAddr + "_" + table_info.getAttr(table_info.getAttrNames()[i]).getName() + ".index";
                im.dropIndex(tmpIndexName);
            }
        }
    }
    //with index, without index..
    else{
        vector<WhereForRecorder>* condNoIndex = new vector<WhereForRecorder>;
        //vector<WhereForRecorder>* condWithIndex = new vector<WhereForRecorder>;
        WhereForRecorder tmp;
        bool index_flag = true;
        /*
         int type;
         string op;
         string value;
         int offset; //the first: 0
         int length;
         */
        for(int i = 0; i < where.size(); i++)
        {
            string attribute_name = where[i].attribute;
            tmp.type = table_info.getAttr(attribute_name).getTypeId();
            tmp.op = where[i].op;
            tmp.value = where[i].parameter;
            int tmp_offset = 0;
            for(int j = 0; j < table_info.getAttrNames().size(); j++)
            {
                if(table_info.getAttrNames()[j] == where[i].attribute)
                    break;
                int type = table_info.getAttr(table_info.getAttrNames()[j]).getTypeId();
                if(type == 0)
                {
                    tmp_offset+=4;
                }
                else if(type == 1)
                {
                    tmp_offset+=4;
                }
                else
                {
                    tmp_offset+=table_info.getAttr(table_info.getAttrNames()[j]).getStrLen();
                }
            }
            tmp.offset = tmp_offset;
            tmp.length = table_info.getAttr(where[i].attribute).getStrLen();
            if(!table_info.getAttr(attribute_name).getHasIndex())
            {
                index_flag = false;
                //condWithIndex->push_back(tmp);
            }
//            else
//            {
                condNoIndex->push_back(tmp);
            //}

            //condNoIndex->push_back(tmp);
        }
        int element_size = 0;
        for(int i = 0; i < table_info.getAttrNames().size(); i++)
        {
            element_size+=table_info.getAllAttrs()[table_info.getAttrNames()[i]].getStrLen();
        }
        //conditionJudge jd(condNoIndex, element_size);
        conditionJudge jdNoIndex(condNoIndex, element_size);
        //conditionJudge jdWithIndex(condWithIndex, element_size);
        if(!index_flag|| where.size() > 1)
        {
            if(!rm.deleteWhereNoIndex(tmpFileAddr, jdNoIndex))
            {
                setSucceed(false);
                cast_command->setInformation("delete failed!!!!!");
                return;
            }
            else{
                cout<<"Success!"<<endl;
            }
        }
        //with one condition and has index
        else{
            vector<int> offset;
            //std::vector<int> findLeft(const AttrType &k);
            //std::vector<int> findInIndex(std::vector<AttrType> datalist)
            int type = condNoIndex->operator[](0).type;
            vector<AttrType> datalist;
            
            
            AttrType attr_value;
            if(type == 0)
            {
                AttrType attr_value(stoi(condNoIndex->operator[](0).value));
            }
            else if(type == 1)
            {
                AttrType attr_value(stof(condNoIndex->operator[](0).value));
            }
            else if(type == 2)
            {
                AttrType attr_value(condNoIndex->operator[](0).value);
            }
            
            datalist.push_back(attr_value);
            if(condNoIndex->operator[](0).op == "=")
            {
                offset = im.findInIndex(datalist);
                conditionJudge jd(condNoIndex, element_size);
                for(int j = 0; j < offset.size(); j++)
                    rm.deleteWhereIndex(tmpFileAddr, offset[j], jd);
                //result = rm.selectWhereIndex(tmpFileAddr, offset, jd);
                //rm.selectWhereNoIndex(tmpFileAddr, jd);
            }
            else if (condNoIndex->operator[](0).op == ">")
            {
                offset = im.btree.findRight(attr_value);
                conditionJudge jd(condNoIndex, element_size);
                for(int j = 0; j < offset.size(); j++)
                    rm.deleteWhereIndex(tmpFileAddr, offset[j], jd);
            }
            else if (condNoIndex->operator[](0).op == "<")
            {
                offset = im.btree.findLeft(attr_value);
                conditionJudge jd(condNoIndex, element_size);
                for(int j = 0; j < offset.size(); j++)
                    rm.deleteWhereIndex(tmpFileAddr, offset[j], jd);
                //result = rm.selectWhereIndex(tmpFileAddr, offset, jd);
                
            }
            else if (condNoIndex->operator[](0).op == ">=")
            {
                offset = im.btree.findRight(attr_value);
                conditionJudge jd(condNoIndex, element_size);
                for(int j = 0; j < offset.size(); j++)
                    rm.deleteWhereIndex(tmpFileAddr, offset[j], jd);
                //result = rm.selectWhereIndex(tmpFileAddr, offset, jd);
                
            }
            else if (condNoIndex->operator[](0).op == "<=")
            {
                offset = im.btree.findLeft(attr_value);
                conditionJudge jd(condNoIndex, element_size);
                for(int j = 0; j < offset.size(); j++)
                    rm.deleteWhereIndex(tmpFileAddr, offset[j], jd);
                //result = rm.selectWhereIndex(tmpFileAddr, offset, jd);
            }
            else if (condNoIndex->operator[](0).op == "!=")
            {
                conditionJudge jd(condNoIndex, element_size);
                rm.deleteWhereNoIndex(tmpFileAddr, jdNoIndex);
                //result = rm.selectWhereNoIndex(tmpFileAddr, jd);
            }
            
        }

        }

    //has index
    //has no index..
    //update index..
    }