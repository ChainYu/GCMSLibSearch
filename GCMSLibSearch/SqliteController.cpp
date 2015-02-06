#include <iostream>
//#include <map>
#include <algorithm>
//#include <assert.h>
#include "time.h"
//#include "match.h"
#include "SqliteController.h"


#define CREATE_TABLE_PEAKDATA	0
#define COUNT_TOTAL_ROWS		1
#define MAX_PEAK_COUNT			2
#define FIND_COMPOUND_BY_ID		3
#define FIND_PEAKDATA_BY_ID		4
#define FIND_COMPOUND_BY_RANK	5
#define FIND_PEAKDATA_BY_RANK	6
#define FIND_PEAKPOINTS_BY_ID	7
#define STORE_COMPOUND_DATA		8
#define STORE_PEAK_DATA			9

#define MAX_MASS 1659

// -init & deinit
SqliteController::SqliteController(const std::string &file): //���Է�ɢ��3���ļ������ǿ��ڲ��ı�ṹһ��
	_ppDB(NULL) {

		const std::string queries[] = { //TODO: CompoundInfo ����ɱ�
			// 0 Create table [PeakData]
			"CREATE TABLE IF NOT EXISTS [PeakData] ([ID] INTEGER PRIMARY KEY AUTOINCREMENT, [CompoundID] INTEGER, [x] INTEGER, [y] INTEGER);"
			"CREATE INDEX IF NOT EXISTS idx_x ON [PeakData] (x);",
			// 1 COUNT TOTAL ROWs 
			"SELECT COUNT(*) FROM CompoundInfo",
			// 2 COUNT MAX PEAK COUNT
			"SELECT max(PeakCount) FROM CompoundInfo LIMIT 1",
			// 3 Find Compound By CompoundID
			"SELECT * FROM CompoundInfo" 
			"  WHERE CompoundID = ?",
			// 4 Find peakData By CompoundID
			"SELECT PeakCount, PeakData FROM CompoundInfo" 
			"  WHERE CompoundID = ?",
			// 5 Find Compounds By Rank
			"SELECT * FROM CompoundInfo"
			"  ORDER BY CompoundID"
			"  LIMIT ?" 
			"  OFFSET ?",
			// 6 Find PeakData By Rank
			"SELECT PeakCount, PeakData FROM CompoundInfo"
			"  ORDER BY CompoundID"
			"  LIMIT ?" 
			"  OFFSET ?",
			// 7 Find PeakPoints By CompoundID
			"SELECT x, y FROM [PeakData]"
			"  WHERE CompoundID = ?"
			"  LIMIT ?", //
			// 8 Store Or update new Compound
			"INSERT OR REPLACE INTO CompoundInfo ([CompoundID], [CompoundName], [Formula], [MassWeight], [CasNo], [PeakCount], [PeakData])"
			" VALUES (?, ?, ?, ?, ?, ?, ?);",
			// 9 Store or update new PeakData
			"INSERT OR REPLACE INTO [PeakData] ([CompoundID], [x], [y])"
			"  VALUES (?, ?, ?);"
		};

		init_openSQLite(file); 

		for (size_t index = 0; index < PREPARED_STATEMENT_COUNT; index++) {
			const std::string query = queries[index];
			sqlite3_prepare_v2(_ppDB, query.c_str(), query.size(), &_pStatements[index], NULL);
		}

		preproccess();
}
SqliteController::~SqliteController(void) { 
	for (size_t index = 0; index < PREPARED_STATEMENT_COUNT; index ++) {
		sqlite3_finalize(_pStatements[index]);
	}
	sqlite3_close(_ppDB);
}
bool SqliteController::init_openSQLite(const std::string &file) {

	int result = sqlite3_open_v2(file.c_str(), &_ppDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	if (result != SQLITE_OK) {
		std::cerr << "init_openSQLite -> sqlite3_open_v2[" << result << "] " << sqlite3_errmsg(_ppDB) << " " << sqlite3_errcode(_ppDB) << std::endl;
		sqlite3_close(_ppDB);
		return false;
	}

	printf("open ---> OK \n\n");

	return true;
}

// - �ⲿ�ӿ��ṩ
void SqliteController::preproccess() {
	// TABLE [PeakData]
	//createPeakDataTable(); 
	//pre_parsePeakDate();

	// TABLE [MassHash]
	//dq_createMassHashTable();
	//dq_pre_buildMassHash();
}
void SqliteController::createPeakDataTable() {

	sqlite3_stmt* statement = _pStatements[CREATE_TABLE_PEAKDATA];
	int rc = sqlite3_step(statement);
	if (rc != SQLITE_OK && rc != SQLITE_DONE) {
		std::cerr << "createPeakDataTable() -> sqlite3_step[" << rc << "] " << sqlite3_errmsg(_ppDB) << " " << sqlite3_errcode(_ppDB) << std::endl;
	}
	sqlite3_reset(statement);
}
// ��
int SqliteController::totalCompoundCounts() { 

	sqlite3_stmt* statement = _pStatements[COUNT_TOTAL_ROWS];
	return query_aSingleCount(statement);

}
int SqliteController::maxPeakCount() {

	sqlite3_stmt* statement = _pStatements[MAX_PEAK_COUNT];
	return query_aSingleCount(statement);
}
Peak SqliteController::getPeakData(int compoundID) {

	Peak aPeak;
	
	sqlite3_stmt* statement = _pStatements[FIND_PEAKDATA_BY_ID];
	int rc = sqlite3_bind_int(statement, 1, compoundID);
	if (rc != SQLITE_OK) {
		std::cerr << "sqlite3_bind_int[" << rc << "] " << sqlite3_errmsg(_ppDB) << " " << sqlite3_errcode(_ppDB) << std::endl;
		return aPeak;
	}

	rc = sqlite3_step(statement);
	if (sqlite3_data_count(statement) > 0) {
		aPeak._peakCount = sqlite3_column_int(statement, 0);
		aPeak._peakData = (const char*)sqlite3_column_text(statement, 1);
	}

	sqlite3_reset(statement);
	return aPeak;
}
std::vector<Peak> SqliteController::getPeakDatas(int startCompoundID, int limit) {
	std::vector<Peak> peaks;
	sqlite3_stmt *statement = _pStatements[FIND_PEAKDATA_BY_RANK];

	if ((sqlite3_bind_int(statement, 1, limit) != SQLITE_OK) ||
		(sqlite3_bind_int(statement, 2, startCompoundID) != SQLITE_OK)) {
			std::cerr << "sqlite3_bind_int" << sqlite3_errmsg(_ppDB) << " " << sqlite3_errcode(_ppDB) << std::endl;
			return peaks;
	}

	sqlite3_exec(_ppDB, "BEGIN;", 0, 0, 0);

	while (sqlite3_step(statement) == SQLITE_ROW) {
		Peak aPeak;
		aPeak._peakCount = sqlite3_column_int(statement, 0);
		aPeak._peakData = (const char*)sqlite3_column_text(statement, 1);
		peaks.push_back(aPeak);
	}

	sqlite3_exec(_ppDB, "COMMIT;", 0, 0, 0);

	sqlite3_reset(statement);
	return peaks;

	/*

	char sql[1024];
	sprintf(sql, "SELECT PeakCount, PeakData FROM CompoundInfo ORDER BY CompoundID LIMIT %d OFFSET %d;", limit, startCompoundID);

	char **pazResult;
	int nRow, nCol;
	int rc = sqlite3_get_table(_ppDB, sql, &pazResult, &nRow, &nCol, 0);
	if (rc != SQLITE_OK) {
	std::cerr << "getPeakDatas - > sqlite3_get_table" << sqlite3_errmsg(_ppDB) << " " << sqlite3_errcode(_ppDB) << std::endl;
	return peaks;
	}

	for (int row = 0; row < nRow; row++) {
	Peak aPeak;
	aPeak._peakCount = atoi(pazResult[nCol * row + nCol]);
	aPeak._peakData = pazResult[nCol * row + nCol + 1];
	peaks.push_back(aPeak);
	}

	sqlite3_free_table(pazResult);

	sqlite3_exec(_ppDB, "COMMIT;", 0, 0, 0);*/
}
Compound SqliteController::getCompound(int compoundID) {

	Compound aCompound;
	sqlite3_stmt *statement = _pStatements[FIND_COMPOUND_BY_ID];

	int rc = sqlite3_bind_int(statement, 1, compoundID);
	if (rc != SQLITE_OK) {
		std::cerr << "sqlite3_bind_int[" << rc << "] " << sqlite3_errmsg(_ppDB) << " " << sqlite3_errcode(_ppDB) << std::endl;
		return aCompound;
	}

	rc = sqlite3_step(statement);
	if (sqlite3_data_count(statement) > 0) {
		aCompound._compoundID = sqlite3_column_int(statement, 0);
		aCompound._compoundName = (const char*)sqlite3_column_text(statement, 1);
		aCompound._formula = (const char*)sqlite3_column_text(statement, 2);
		aCompound._massWeight = sqlite3_column_int(statement, 3);
		aCompound._casNo = (const char*)sqlite3_column_text(statement, 4);
		aCompound._peakCount = sqlite3_column_int(statement, 5);
		aCompound._peakData = (const char*)sqlite3_column_text(statement, 7); // Col 6 = `Comment`
	}
	sqlite3_reset(statement);
	return aCompound;
}
std::vector<Compound> SqliteController::getCompounds(int startCompoundID, int limit) {

	std::vector<Compound> compounds;
	sqlite3_stmt *statement = _pStatements[FIND_COMPOUND_BY_RANK];

	if ((sqlite3_bind_int(statement, 1, limit) != SQLITE_OK) ||
		(sqlite3_bind_int(statement, 2, startCompoundID) != SQLITE_OK)) {
		std::cerr << "sqlite3_bind_int" << sqlite3_errmsg(_ppDB) << " " << sqlite3_errcode(_ppDB) << std::endl;
		return compounds;
	}

	while (sqlite3_step(statement) == SQLITE_ROW) {
		Compound aCompound;
		aCompound._compoundID = sqlite3_column_int(statement, 0);
		aCompound._compoundName = (const char*)sqlite3_column_text(statement, 1);
		aCompound._formula = (const char*)sqlite3_column_text(statement, 2);
		aCompound._massWeight = sqlite3_column_int(statement, 3);
		aCompound._casNo = (const char*)sqlite3_column_text(statement, 4);
		aCompound._peakCount = sqlite3_column_int(statement, 5);
		aCompound._peakData = (const char*)sqlite3_column_text(statement, 7); // Col 6 = `Comment`
		compounds.push_back(aCompound);
	}

	sqlite3_reset(statement);
	return compounds;
}
void SqliteController::getPeakPoints(int compoundID, int pointCount, unsigned int* x, float* y) {

	sqlite3_stmt *statement = _pStatements[FIND_PEAKPOINTS_BY_ID];

	if ((sqlite3_bind_int(statement, 1, compoundID) != SQLITE_OK) ||
		(sqlite3_bind_int(statement, 2, pointCount) != SQLITE_OK)) {
		std::cerr << "getPeakPoints -> sqlite3_bind_int" << sqlite3_errmsg(_ppDB) << " " << sqlite3_errcode(_ppDB) << std::endl;	
	}

	int i = 0;
	while (sqlite3_step(statement) == SQLITE_ROW) {
		unsigned int x_ = static_cast<unsigned int>(sqlite3_column_int(statement, 0));
		float y_ = static_cast<float>(sqlite3_column_int(statement, 1));

		x[i] = x_;
		y[i] = y_;

		i++;
	}

	if (i != pointCount) {
		std::cerr << "pointSize goes wrong -> compoundID =" << compoundID << std::endl;	
	}
	sqlite3_reset(statement);
}
// �� / ��
void SqliteController::storeCompound(const Compound& aCompound) {

	// TODO: VERIFY compound datas
	sqlite3_stmt *statement = _pStatements[STORE_COMPOUND_DATA];
	
	if ((sqlite3_bind_int(statement,  1, aCompound._compoundID) == SQLITE_OK) &&
		(sqlite3_bind_text(statement, 2, aCompound._compoundName.c_str(), -1, SQLITE_STATIC) == SQLITE_OK) &&
		(sqlite3_bind_text(statement, 3, aCompound._formula.c_str(), -1, SQLITE_STATIC) == SQLITE_OK) &&
		(sqlite3_bind_int(statement,  4, aCompound._massWeight) == SQLITE_OK) &&
		(sqlite3_bind_text(statement, 5, aCompound._casNo.c_str(), -1, SQLITE_STATIC) == SQLITE_OK) &&
		(sqlite3_bind_int(statement,  6, aCompound._peakCount) == SQLITE_OK) &&
		(sqlite3_bind_text(statement, 7, aCompound._peakData.c_str(), -1, SQLITE_STATIC) == SQLITE_OK)) {

		   sqlite3_step(statement);
		   sqlite3_reset(statement);
	}
}
void SqliteController::storePeakData(const PeakPoint& aPoint) {

	sqlite3_stmt *statement = _pStatements[STORE_PEAK_DATA];

	if ((sqlite3_bind_int(statement, 1, aPoint._compoundID) == SQLITE_OK) &&
		(sqlite3_bind_int(statement, 2, aPoint._x) == SQLITE_OK) &&
		(sqlite3_bind_int(statement, 3, aPoint._y) == SQLITE_OK) ) {

			sqlite3_step(statement);
			sqlite3_reset(statement);
	}

}
// - �ڲ��ӿ�
void SqliteController::queryCompoundData(std::vector<Compound> &selectedCompounds) {

	if (!selectedCompounds.empty()) { selectedCompounds.clear(); }


	//TODO: ʵ�ʼ������ݴ��������������
	int randomID[20] = { 0 };
	srand((unsigned)time(NULL));
	for (int i = 0; i < sizeof(randomID) / sizeof(int); i++) {
		randomID[i] = rand() % 191436 + 1;
		Compound aCompound = getCompound(randomID[i]);
		selectedCompounds.push_back(aCompound);
	}	

	return;
}
int  SqliteController::query_aSingleCount(sqlite3_stmt* pStatement) {
	int count = 0;
	int rc = sqlite3_step(pStatement);
	if (rc != SQLITE_ROW) {
		std::cerr << "query_aSingleCount -> sqlite3_step[" << rc << "] " << sqlite3_errmsg(_ppDB) << " " << sqlite3_errcode(_ppDB) << std::endl;
		return -1;
	}

	count = sqlite3_column_int(pStatement, 0);
	sqlite3_reset(pStatement);
	return count;
}
void SqliteController::pre_parsePeakDataString(const std::string& strPeakData, int peakCount, int *x, int *y) {
	std::string::size_type i = 0;
	std::string::size_type j = strPeakData.find(';');

	for (int index = 0; index < peakCount; index++) {

		std::string strXY = strPeakData.substr(i, j-i);
		std::string::size_type w = strXY.find(' ');
		std::string strX = strXY.substr(0, w);
		std::string strY = strXY.substr(w+1, strXY.length());

		x[index] = atoi(strX.c_str());
		y[index] = (float)atoi(strY.c_str());

		i = ++j;
		j = strPeakData.find(';', j);

	}
}
void SqliteController::pre_parsePeakDataString(const std::string& strPeakData, int peakCount, std::vector<PeakPoint> &peakPoints) {

	std::string::size_type i = 0;
	std::string::size_type j = strPeakData.find(';');

	for (int index = 0; index < peakCount; index++) {

		std::string strXY = strPeakData.substr(i, j-i);
		std::string::size_type w = strXY.find(' ');
		std::string strX = strXY.substr(0, w);
		std::string strY = strXY.substr(w+1, strXY.length());

		PeakPoint aPeakPoint;
		aPeakPoint._x = atoi(strX.c_str());
		aPeakPoint._y = atoi(strY.c_str());
		peakPoints.push_back(aPeakPoint);

		i = ++j;
		j = strPeakData.find(';', j);

	}
}
void SqliteController::pre_parsePeakDate() {

	sqlite3_exec(_ppDB, "BEGIN;", 0, 0, 0);

	int* x = new int[800];
	int* y = new int[800];

	const int totalCompounds = totalCompoundCounts();
	for (int compoundID = 1; compoundID < totalCompounds; compoundID++) {

		if (compoundID % 10000 == 0) { 
			std::cout << compoundID << " done." << std::endl;
		}

		Peak aPeak = getPeakData(compoundID);
		const std::string strPeakData = aPeak._peakData;
		const int peakCount = aPeak._peakCount;

		pre_parsePeakDataString(strPeakData, peakCount, x, y);

		for (int peakIndex = 0; peakIndex < peakCount; peakIndex++) {
			PeakPoint aPoint;
			aPoint._compoundID = compoundID;
			aPoint._x = x[peakIndex];
			aPoint._y = y[peakIndex];
			storePeakData(aPoint);
		}
	}

	sqlite3_exec(_ppDB, "COMMIT;", 0, 0, 0);

	std::cout << "all " << totalCompounds << " done." << std::endl;
	delete [] y;
	delete [] x;
}
void SqliteController::parseCompoundIDs(const std::string &strCompoundIDs, std::set<int> &compoundIDs) {

	std::string::size_type i = 0;
	std::string::size_type j = strCompoundIDs.find(',');

	while ( j != std::string::npos) {

		std::string strCompoundID = strCompoundIDs.substr(i, j-i);
		compoundIDs.insert( atoi(strCompoundID.c_str()) );

		i = ++j;
		j = strCompoundIDs.find(',', j);
	}
}
// Dirty & Quick
std::vector<int> SqliteController::dq_getAllPeakCounts() {
	std::vector<int> counts;

	const std::string query = "SELECT PeakCount FROM CompoundInfo";
	sqlite3_stmt *statement;
	sqlite3_prepare_v2(_ppDB, query.c_str(), query.size(), &statement, NULL);

	
	while (sqlite3_step(statement) == SQLITE_ROW) {
		int count = sqlite3_column_int(statement, 0);
		counts.push_back(count);
	}

	sqlite3_finalize(statement);
	return counts;
}
void SqliteController::dq_createMassHashTable() {
	sqlite3_stmt* statement;
	const std::string query = "CREATE TABLE IF NOT EXISTS [MassHash] ([Mass] INTEGER PRIMARY KEY, [IDs] CHAR);";
	sqlite3_prepare_v2(_ppDB, query.c_str(), query.size(), &statement, NULL);

	int rc = sqlite3_step(statement);
	if (rc != SQLITE_OK && rc != SQLITE_DONE) {
		std::cerr << "dq_createMassHashTable() -> sqlite3_step[" << rc << "] " << sqlite3_errmsg(_ppDB) << " " << sqlite3_errcode(_ppDB) << std::endl;
	}
	sqlite3_finalize(statement);
}
void SqliteController::dq_pre_buildMassHash() {

	sqlite3_stmt* statement;
	const std::string queryCount = "SELECT COUNT(Mass) FROM [MassHash]";
	sqlite3_prepare_v2(_ppDB, queryCount.c_str(), queryCount.size(), &statement, NULL);
	int massCount = 0;
	if (sqlite3_step(statement) == SQLITE_ROW) {
		massCount = sqlite3_column_int(statement, 0);
	}
	sqlite3_finalize(statement);

	// ��������� Mass ���
	if (massCount == 0) {
		const std::string query = "INSERT OR REPLACE INTO [MassHash](Mass, IDs) VALUES (?, '')";
		sqlite3_prepare_v2(_ppDB, query.c_str(), query.size(), &statement, NULL);

		sqlite3_exec(_ppDB, "BEGIN;", 0, 0, 0);
		for (int i=1; i <= MAX_MASS; i++) {
			if (sqlite3_bind_int(statement, 1, i) == SQLITE_OK) {
				sqlite3_step(statement);
				sqlite3_reset(statement);
			}
		}
		sqlite3_exec(_ppDB, "COMMIT;", 0, 0, 0);
		sqlite3_finalize(statement);
		std::cout << "dq_pre_buildMassHash() -> OK" << std::endl;
	}

	// ��� IDs 	
	
	sqlite3_stmt* statementPeakData;
	const std::string queryPeakData = "SELECT [compoundID] FROM [PeakData] WHERE x = ?;";

	sqlite3_stmt* statementMassHash;
	//const std::string queryMassHash = "SELECT [IDs] FROM [MassHash] WHERE Mass = ? LIMIT 1";
	const std::string insertMassHash = "INSERT OR REPLACE INTO [MassHash](Mass, IDs) VALUES (?, ?)";


	//std::map<int, std::string> massHash; // mass - id
	//for (int i=1; i <= MAX_MASS; i++) { massHash[i] = ""; }

	
	for (int massIndex = 1; massIndex <= MAX_MASS; massIndex++) {

		// ��ȡPeakData �еĶ�Ӧ��ϵ
		int count = 0;
		double timeStart = (double)clock();

		std::string compoundIDs = "";

		sqlite3_exec(_ppDB, "BEGIN;", 0, 0, 0);
		sqlite3_prepare_v2(_ppDB, queryPeakData.c_str(), queryPeakData.size(), &statementPeakData, NULL);
		if (sqlite3_bind_int(statementPeakData, 1, massIndex) == SQLITE_OK) {

			while (sqlite3_step(statementPeakData) == SQLITE_ROW) {

				compoundIDs += (const char*)sqlite3_column_text(statementPeakData, 0); 
				compoundIDs += ",";
				count++;
			}
		}
		sqlite3_exec(_ppDB, "COMMIT;", 0, 0, 0);
		sqlite3_finalize(statementPeakData);

		// ����MassHash
		sqlite3_prepare_v2(_ppDB, insertMassHash.c_str(), insertMassHash.size(), &statementMassHash, NULL);
		if ((sqlite3_bind_int(statementMassHash, 1, massIndex) == SQLITE_OK) &&
			(sqlite3_bind_text(statementMassHash, 2, compoundIDs.c_str(), -1, SQLITE_STATIC) == SQLITE_OK) ) {

				sqlite3_step(statementMassHash);
				sqlite3_finalize(statementMassHash);
		}

		double timeFinish = (double)clock();
		std::cout << massIndex << " done ("<< count <<") "<< MAX_MASS - massIndex << " to go. =" << timeFinish-timeStart <<" ms" << std::endl;
		
	}
}

void SqliteController::dq_filterPeakByTwoMass(const Compound &aCompound, std::set<int> &compoundIDsSet) {

	const std::string strPeakData = aCompound._peakData;
	const int peakCount = aCompound._peakCount;

	// �����ַ��� �ҳ����ʹ�����Yֵ��Ӧ��X
	
	std::vector<PeakPoint> peakPoints;
	pre_parsePeakDataString(strPeakData, peakCount, peakPoints);
	if (peakPoints.size() < 2) { return; }

	nth_element(peakPoints.begin(), peakPoints.begin() + 2, peakPoints.end(), SqliteController::peakPointCompare); // ����2����
	int maxX = peakPoints[0]._x; 
	int subX = peakPoints[1]._x;

	//std::string strCompoundIDs = "";
	sqlite3_stmt* statement;
	const std::string query = "SELECT IDs FROM MassHash WHERE Mass = ? LIMIT 1;"; // TODO: �ֳ�2��
	sqlite3_prepare_v2(_ppDB, query.c_str(), query.size(), &statement, NULL);

	//std::set<int> compoundIDsSet;
	if ((sqlite3_bind_int(statement, 1, maxX) == SQLITE_OK)) {
		if (sqlite3_step(statement) == SQLITE_ROW) {
			const std::string strCompoundIDs = (const char*)sqlite3_column_text(statement, 0);
			parseCompoundIDs(strCompoundIDs, compoundIDsSet);
			sqlite3_reset(statement);
		}
	}

	if ((sqlite3_bind_int(statement, 1, subX) == SQLITE_OK)) {
		if (sqlite3_step(statement) == SQLITE_ROW) {
			const std::string strCompoundIDs = (const char*)sqlite3_column_text(statement, 0);
			parseCompoundIDs(strCompoundIDs, compoundIDsSet);
			sqlite3_reset(statement);
		}
	}
}

