#pragma once

//#include "StdAfx.h" //
#include "sqlite3.h"
#include "Compound.h"
#include <vector>
#include <set>

#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "sqlite3_D.lib")
#else
#pragma comment(lib, "sqlite3_R.lib")
#endif


#define PREPARED_STATEMENT_COUNT 9
#define COUNT_COMPOUNDS 191438
#define COMPOUNDS_SIZES 191439

class SqliteController {

public:
	SqliteController(const std::string &file);
	~SqliteController(void);

//�ӿ�
	// Ԥ����
	void SqliteController::preproccess();

	// ��
	void SqliteController::createPeakDataTable(); // +Index
	void SqliteController::dq_createMassHashTable(); // +Index
	void SqliteController::dq_createCompoundTable(); //

	// ��
	int SqliteController::totalCompoundCounts(); //����������
	int SqliteController::maxPeakCount(); //�����������
	Compound SqliteController::getCompound(int compoundID); //��ID��ö�Ӧ������
	std::vector<Compound> SqliteController::getCompounds(int startCompoundID, int limit); //����ʼID��limit ��ȡ��Ӧ������
	void SqliteController::queryCompoundData(std::vector<Compound> &selectedCompounds);
	void SqliteController::getPeakData(int compoundID, Peak &aPeak); //��ID��ö�Ӧ������������
	std::vector<Peak> SqliteController::getPeakDatas(int startCompoundID, int limit);
	void SqliteController::dq_getPeakDatas(std::set<int> &compoundIDsSet, std::vector<Peak>& peaks);
	void SqliteController::dq_getPeakDatas_v2(int*, std::vector<Peak>& peaks);
	void SqliteController::dq_getPeakDatas_v3(int*, std::vector<Peak>& peaks);
	void SqliteController::getPeakPoints(int compoundID, unsigned int* x, float* y);
	void SqliteController::dq_getPeakPoints(int compoundID, std::vector<PeakXY>& peakXYs);
	void SqliteController::dq_getPeakPoints(std::vector<PeakPoint>& peakPoints);

	// ��/��
	void SqliteController::storeCompound(const Compound& aCompound); //��������������
	void SqliteController::storePeakData(const PeakPoint& aPoint);

	// ɾ
	
	
	//Dirty & Quick
	std::vector<int> SqliteController::dq_getAllPeakCounts();
	void SqliteController::dq_filterPeakByTwoMass(const Compound &aCompound, int* compoundIDs);
	void SqliteController::dq_filterPeakByMaxX(const int maxX, int* compoundIDs);
	void SqliteController::dq_pre_buildMassHash();


	void SqliteController::dq_pre_buildCompound(std::vector<Compound> &compounds);
	void SqliteController::dq_pre_getCompoundInfo(std::vector<Compound> &compounds);


	bool SqliteController::init_openSQLite(const std::string &file);
	bool SqliteController::checkConnectionError();

	int  SqliteController::query_aSingleCount(sqlite3_stmt* pStatement);
	// Parse TODO�� �ϳ�һ������
	void SqliteController::pre_parsePeakDate(); //��CompoundInfo���PeakData�ֶ����ݽ�������PeakData����
	void SqliteController::pre_parsePeakDataString(const std::string& strPeakData, int peakCount, int *x, int *y);
	void SqliteController::pre_parsePeakDataString(const std::string& strPeakData, int peakCount, std::vector<PeakPoint> &peakPoints);
	int SqliteController::pre_parsePeakDataString(const std::string& strPeakData, int peakCount);
	//void SqliteController::parseCompoundIDs(const std::string &strCompoundIDs, std::set<int> &compoundIDs);
	void SqliteController::parseCompoundIDs(const std::string &strCompoundIDs, int* compoundIDs);


private:
	sqlite3* _ppDB;
	sqlite3_stmt* _pStatements[PREPARED_STATEMENT_COUNT];

	static bool peakPointCompare(const PeakPoint &p1, const PeakPoint &p2) {  return p1._y > p2._y; }  
};

