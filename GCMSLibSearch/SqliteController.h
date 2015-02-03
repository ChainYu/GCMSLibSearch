#pragma once

//#include "StdAfx.h" //
#include "sqlite3.h"
#include "Compound.h"
#include <vector>

#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "sqlite3_D.lib")
#else
#pragma comment(lib, "sqlite3_R.lib")
#endif



#define PREPARED_STATEMENT_COUNT 5



class SqliteController {

public:
	SqliteController(const std::string &file);
	~SqliteController(void);

//�ӿ�
	int SqliteController::totalCompoundCounts(); //����������
	int SqliteController::maxPeakCount(); //�����������
	Compound SqliteController::getCompound(int compoundID); //��ID��ö�Ӧ������
	std::vector<Compound> SqliteController::getCompounds(int startCompoundID, int limit); //����ʼID��limit ��ȡ��Ӧ������
	void SqliteController::storeCompound(const Compound& aCompound); //��������������

	//ɾ��������

	//bool hasCompoundRecords(const std::string &tableName = TABLE_COMPOUND_INFO); //�������ݿ�����Ƿ���ڻ���������



	void queryCompoundData(std::vector<Compound> &selectedCompounds);
	

private:
	bool init_openSQLite(const std::string &file);


private:
	sqlite3* _ppDB;
	sqlite3_stmt* _pStatements[PREPARED_STATEMENT_COUNT];
};

