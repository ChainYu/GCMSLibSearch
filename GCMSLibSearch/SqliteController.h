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



#define PREPARED_STATEMENT_COUNT 10



class SqliteController {

public:
	SqliteController(const std::string &file);
	~SqliteController(void);

//�ӿ�
	// Ԥ����
	void SqliteController::preproccess();

	// ��
	void SqliteController::createPeakDataTable();
	void SqliteController::dq_createMassHashTable();

	// ����
	//void SqliteController::dq_createIndexOnMassHash();

	// ��
	int SqliteController::totalCompoundCounts(); //����������
	int SqliteController::maxPeakCount(); //�����������
	Compound SqliteController::getCompound(int compoundID); //��ID��ö�Ӧ������
	std::vector<Compound> SqliteController::getCompounds(int startCompoundID, int limit); //����ʼID��limit ��ȡ��Ӧ������
	Peak SqliteController::getPeakData(int compoundID); //��ID��ö�Ӧ������������
	std::vector<Peak> SqliteController::getPeakDatas(int startCompoundID, int limit);

	void SqliteController::getPeakPoints(int compoundID, int pointCount, unsigned int* x, float* y);
	

	// ��/��
	void SqliteController::storeCompound(const Compound& aCompound); //��������������
	void SqliteController::storePeakData(const PeakPoint& aPoint);

	// ɾ
	//bool hasCompoundRecords(const std::string &tableName = TABLE_COMPOUND_INFO); //�������ݿ�����Ƿ���ڻ���������



	void queryCompoundData(std::vector<Compound> &selectedCompounds);
	
	//Dirty & Quick
	std::vector<int> SqliteController::dq_getAllPeakCounts();
	std::vector<int> SqliteController::dq_peakFilterByTwoMass();
	void SqliteController::dq_pre_buildMassHash();

private:
	bool SqliteController::init_openSQLite(const std::string &file);
	int  SqliteController::query_aSingleCount(sqlite3_stmt* pStatement);
	void SqliteController::pre_parsePeakDate(); //��CompoundInfo���PeakData�ֶ����ݽ�������PeakData����
	void SqliteController::pre_parsePeakDataString(const std::string& strPeakData, int peakCount, int *x, int *y);


	

private:
	sqlite3* _ppDB;
	sqlite3_stmt* _pStatements[PREPARED_STATEMENT_COUNT];
};

