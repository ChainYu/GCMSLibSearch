#include "StdAfx.h"
#include "SqliteSearchController.h"
#include <iostream>
#include <vector>
#include "time.h"

SqliteSearchController::SqliteSearchController(void) {
	_ppDB = NULL;
}


SqliteSearchController::~SqliteSearchController(void)
{
}


bool SqliteSearchController::connectSQLite() {

	int result = sqlite3_open("..//ms.db", &_ppDB);

	if (result != SQLITE_OK) {
		fprintf(stderr, "can't open db!\n", sqlite3_errmsg(_ppDB));
		sqlite3_close(_ppDB);
		exit(1);
		return false;
	}
		
	printf("db open successfully!\n");
	return true;
}

void SqliteSearchController::queryCompoundData() {

	if (!connectSQLite()) { return; }

	//TODO: ʵ�ʼ������ݴ��������������
	int randomID[20] = { 0 };
	srand((unsigned)time(NULL));
	for (int i = 0; i < sizeof(randomID) / sizeof(int); i++) {
		randomID[i] = rand() % 191436 + 1;
	}
	
	Compound compound = getCompound(6);
	
	closeSQLite();
	return;
}

Compound SqliteSearchController::getCompound(int compoundID) {

	Compound aCompound;
	int rc = 0;
	sqlite3_stmt *stmt;
	std::string query = "SELECT * FROM CompoundInfo WHERE CompoundID = ?";

	rc = sqlite3_prepare(_ppDB, query.c_str(), query.size(), &stmt, NULL);
	if (rc != SQLITE_OK) {

		std::cerr << "sqlite3_prepare[" << rc << "] " << sqlite3_errmsg(_ppDB) << " " << sqlite3_errcode(_ppDB) << std::endl << query << std::endl;
		sqlite3_finalize(stmt);
		return aCompound;
	}

	rc = sqlite3_bind_int(stmt, 1, compoundID);
	if (rc != SQLITE_OK) {
		std::cerr << "sqlite3_bind_int[" << rc << "] " << sqlite3_errmsg(_ppDB) << " " << sqlite3_errcode(_ppDB) << std::endl << query << std::endl;
		sqlite3_finalize(stmt);
		return aCompound;
	}

	rc = sqlite3_step(stmt);
	if (rc == SQLITE_DONE) { return aCompound; }

	
	if (sqlite3_data_count(stmt) > 0) {
		aCompound._compoundID = sqlite3_column_int(stmt, 0);
		aCompound._compoundName = (const char*)sqlite3_column_text(stmt, 1);
		aCompound._formula = (const char*)sqlite3_column_text(stmt, 2);
		aCompound._massWeight = sqlite3_column_int(stmt, 3);
		aCompound._casNo = (const char*)sqlite3_column_text(stmt, 4);
		aCompound._peakCount = sqlite3_column_int(stmt, 5);
		aCompound._peakData = (const char*)sqlite3_column_text(stmt, 7); // Col 6 = `Comment`
	}

	sqlite3_finalize(stmt);
	return aCompound;
}


void SqliteSearchController::closeSQLite() {
	if (_ppDB) {
		sqlite3_close(_ppDB);
	}
	
}