#include "catalog_manager.h"

void CatalogManager::createTableInfo(const TableInfo& tableInfo) {
	std::ofstream tableListFile("/Users/jason/Desktop/tableList.txt", std::ios::app); // append
	std::string tableName = tableInfo.getName();
	tableListFile << tableName << std::endl;
	tableListFile.close();

	writeTableInfo(tableInfo);
}

void CatalogManager::dropTableInfo(std::string tableName) {
	// delete the table from the table list first
	std::ifstream tableListInFile("tableList.txt");
	std::string tmpStr;
	std::vector<std::string> tableList;

	while( std::getline(tableListInFile, tmpStr) ) {
		if( tmpStr != tableName ) {
			tableList.push_back(tmpStr);
		}
	}

	tableListInFile.close();

	std::ofstream tableListOutFile("tableList.txt", std::ios::trunc);
	std::vector<std::string>::iterator it;

	for( it = tableList.begin(); it != tableList.end(); it++ ) {
		tableListOutFile << *it;
	}

	tableListOutFile.close();

	// delete table info
	tmpStr = "del "+tableName+".table";
	system(tmpStr.c_str());
}


TableInfo CatalogManager::getTableInfo(const std::string& tableName) {
	std::string tmpStr = tableName+".table";
	std::ifstream tableFile(tmpStr.c_str());
	TableInfo tableInfo;
	
	bool isFirstLine = true;

	std::string attrName;
	int typeId;
	int strLen;
	bool isUnique;
	bool isPrimaryKey;
	bool hasIndex;

	while( std::getline(tableFile, tmpStr) ) {
		std::stringstream tmpLineStream(tmpStr);
		if( isFirstLine ) {
			tableInfo.setName(tmpStr);
			isFirstLine = false;
		} else {
			tmpLineStream >> attrName >> typeId >> strLen >> isUnique >> isPrimaryKey >> hasIndex;
			AttrInfo attrInfo(attrName, typeId, strLen, isUnique, isPrimaryKey, hasIndex);
			tableInfo.addAttr(attrName, attrInfo);
		}
	}


	tableFile.close();

	return tableInfo;
}


void CatalogManager::writeTableInfo(const TableInfo& tableInfo) {
	std::string tableName = tableInfo.getName();
	std::string tmpStr = "/Users/jason/Desktop/"+tableName+".table";
	std::ofstream tableFile(tmpStr.c_str());

	// write table name
	tableFile << tableInfo.getName() << std::endl;

	// write attribute info
	std::map<std::string, AttrInfo>::iterator it;
	std::map<std::string, AttrInfo> attrs = tableInfo.getAllAttrs();
	for( it = attrs.begin(); it != attrs.end(); it++ ) {
		tableFile << it->second.getName() << " " << it->second.getTypeId() << " " << it->second.getStrLen() << " " << it->second.getIsUnique() << " " << it->second.getIsPrimaryKey() << " " << it->second.getHasIndex() << " " << std::endl;
	}

	tableFile.close();
}

