#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include <math.h>
#include "time.h"
#include "match.h"
#include "SqliteController.h"


//#define  LIBMANAGERAPI __declspec(dllexport)

#define MAX_PEAK_COUNT	800
#define MAX_MASS		1659
#define MAX_Y_TO_X_EQUAL  1
#define SUB_Y_TO_X_EQUAL  2
#define FILTER_LIMIT      3  //滤过后几重交集
#define COMPOUND_FOUND	1024
#define FILTER_PEAK_LIMIT_TIMES 2 //过滤轮数
#define RANK_UPPER	FILTER_PEAK_LIMIT_TIMES+7 // rank的范围[1，9]

// -Table
#define CREATE_TABLE_PEAKDATA "CREATE TABLE IF NOT EXISTS [PeakData] ([ID] INTEGER PRIMARY KEY AUTOINCREMENT, [CompoundID] INTEGER, [x] INTEGER, [y] INTEGER);"
#define CREATE_TABLE_MASSHASH "CREATE TABLE IF NOT EXISTS [MassHash] ([Mass] INTEGER PRIMARY KEY, [IDs] CHAR);"
#define CREATE_TABLE_COMPOUND "CREATE TABLE IF NOT EXISTS [Compound] ([CompoundID] INTEGER PRIMARY KEY, [CompoundName] TEXT COLLATE RTRIM, [Formula] TEXT COLLATE NOCASE, [MassWeight] INTEGER, [CasNo] TEXT, [PeakCount] INTEGER, [MaxX] INTEGER, [PeakData] TEXT);"
#define CREATE_TABLE_FILTER	  "CREATE TABLE IF NOT EXISTS [Filter] ([CompoundID] INTEGER, [X] INTEGER, [Y] INTEGER, [YrX] INTEGER, [Rank] INTEGER);"

// -Index
#define CREATE_INDEX_X_ON_PEAKDATA "CREATE INDEX IF NOT EXISTS idx_x ON [PeakData] (x);"
//#define CREATE_INDEX_ID_ON_PEAKDATA "CREATE INDEX IF NOT EXISTS idx_id ON [PeakData] (CompoundID);"
#define CREATE_INDEX_MASSHASH "CREATE INDEX IF NOT EXISTS idx_mass ON [MassHash] (Mass);"
#define CREATE_INDEX_MAXX_ON_COMPOUND "CREATE INDEX IF NOT EXISTS idx_maxx ON [Compound] (MaxX);"
#define CREATE_INDEX_YRX_ON_FILTER "CREATE INDEX IF NOT EXISTS idx_yrx ON [Filter] (YrX)"
#define CREATE_INDEX_X_ON_FILTER "CREATE INDEX IF NOT EXISTS idx_x ON [Filter] (X)"
#define CREATE_INDEX_RANK_ON_FILTER "CREATE INDEX IF NOT EXISTS idx_rank ON [Filter] (YrX, Rank)"
// -Count
#define COUNT_MASS_ROWS  "SELECT COUNT(Mass) FROM [MassHash]"
#define COUNT_TOTAL_ROWS "SELECT COUNT(CompondID) FROM CompoundInfo"
// -Select
#define SELECT_XY_FROM_PEAKDATA  "SELECT x, y FROM [PeakData] WHERE CompoundID = ?"
#define SELECT_MAX_PEAK_COUNT "SELECT max(PeakCount) FROM Compound LIMIT 1"
#define SELECT_MAX_COMPOUND_ID "SELECT max(CompoundID) FROM Compound LIMIT 1"
#define SELECT_COMPOUND_BY_ID "SELECT * FROM Compound WHERE CompoundID = ?"
#define SELECT_PEAKDATA_BY_ID "SELECT PeakCount, PeakData FROM CompoundInfo WHERE CompoundID = ? LIMIT 1"
#define SELECT_COMPOUND_BY_RANK "SELECT * FROM CompoundInfo ORDER BY CompoundID LIMIT ? OFFSET ?"
#define SELECT_PEAKDATA_BY_RANK	"SELECT PeakCount, PeakData FROM CompoundInfo ORDER BY CompoundID LIMIT ? OFFSET ?"
#define SELECT_PEAKPOINTS_BY_ID	"SELECT x, y FROM [PeakData] WHERE CompoundID = ? LIMIT ?"
// -Insert
#define INSERT_COMPOUND_DATA  "INSERT OR REPLACE INTO [Compound] ([CompoundID], [CompoundName], [Formula], [MassWeight], [CasNo], [PeakCount], [MaxX], [PeakData]) VALUES (?, ?, ?, ?, ?, ?, ?, ?);"
#define INSERT_PEAK_DATA	  "INSERT OR REPLACE INTO [PeakData] ([CompoundID], [x], [y]) VALUES (?, ?, ?);"
#define INSERT_FILTER_DATA	  "INSERT OR REPLACE INTO [Filter] ([CompoundID], [X], [Y], [YrX], [Rank]) VALUES (?, ?, ?, ?, ?);"
// -Delect
#define DELETE_COMPOUND_BY_ID	"DELETE FROM [Compound] WHERE CompoundID = ?"
#define DELETE_FILTER_BY_ID		"DELETE FROM [Filter] WHERE CompoundID = ?"

// -init & deinit
SqliteController::SqliteController(const std::string &file): //可以分散成3个文件。但是库内部的表结构一致
	_ppDB(NULL) { //TODO: 表必须已经存在

		init_openSQLite(file); 
		pre_proccess();
}
SqliteController::~SqliteController(void) { 
	sqlite3_close(_ppDB);
}
bool SqliteController::init_openSQLite(const std::string &file) {

	std::string asciiFile = file;
	std::string utf8File = ascii2utf8(asciiFile);

	// 如果是中文名必须先转码
	int result = sqlite3_open_v2(utf8File.c_str(), &_ppDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	if (result != SQLITE_OK) {
		std::cerr << "init_openSQLite -> sqlite3_open_v2[" << result << "] " << sqlite3_errmsg(_ppDB) << " " << sqlite3_errcode(_ppDB) << std::endl;
		sqlite3_close(_ppDB);
		return false;
	}

	printf("open --> OK \n\n");

	return true;
}
bool SqliteController::checkConnectionError() {
	int code = sqlite3_errcode(_ppDB);
	if ((code > SQLITE_OK) && (code < SQLITE_ROW)) {
		const char* message = sqlite3_errmsg(_ppDB);
		std::cout << "SQLite error: " << message << std::endl;
		return true;
	}
	return false;
}


// - DLL
void SqliteController::libSearch(const LibConfig& libConfig, Compound testCompound, std::vector<Compound> &libCompounds) {

	const int matchLimitNumber = libConfig._matchLimitNumber; //匹配个数上限
	bool isUniqueCAS = libConfig._isUnique;

	if (!libCompounds.empty()) { libCompounds.clear(); }

	// 【match compound】
	const int matchPeakCount = testCompound._peakCount;
	unsigned int* matchX = new unsigned int[matchPeakCount];
	float* matchY = new float[matchPeakCount];
	parseCompound(testCompound, matchX, matchY);

	// 【filtered compounds】 这里有一套判断的逻辑，最终输出就是过滤后剩余的 compoundID 数组
	int *compoundIDs = new int[MAX_COMPOUND_ID + 1](); // [0]存放个数
	//dq_filterCompounds(testCompound, compoundIDs);
	filterCompounds(testCompound, compoundIDs);

	// 【SQLite Search】
	std::vector<Peak> peaks;
	getPeaksByCompoundIDs(compoundIDs, peaks); 

	// lib compound
	unsigned int* libX = new unsigned int[MAX_PEAK_COUNT];
	float* libY = new float[MAX_PEAK_COUNT];

	
	const size_t peakSize = peaks.size();
	for (size_t i = 0; i != peakSize; i++) {

		//Parse String
		parsePeakData(peaks[i]._peakData, peaks[i]._peakCount, libX, libY);
		
		// Diff Algorithm
		peaks[i]._matchDegree = DiffSpectrum(matchX, matchY, matchPeakCount, libX, libY, peaks[i]._peakCount);
	}


	// 【Get result】
	if (peakSize > 0) {
		nth_element(peaks.begin(), peaks.begin() + peaks.size() - 1, peaks.end(), 
			SqliteController::peakCompare_MatchDegree);
	}
	

	bool findRepeat = false;
	for (size_t j = 0; j < matchLimitNumber && j != peaks.size(); j++) {
		int compoundID = peaks[j]._compoundID;
		Compound aCompound = getCompound(compoundID);
		aCompound._matchDegree = peaks[j]._matchDegree;

		// 剔除重复
		for (size_t k = 0; k != libCompounds.size() && isUniqueCAS; k++) {
			if ((aCompound._casNo == libCompounds[k]._casNo) && (aCompound._casNo != "")) {
				findRepeat = true;
				break;
			}
		}

		if (!findRepeat || !isUniqueCAS) {
			libCompounds.push_back(aCompound);
		}

		findRepeat = false;
	}


	delete [] compoundIDs;
	delete [] libY;
	delete [] libX;
	delete [] matchY;
	delete [] matchX;

	peaks.clear(); 
}


// - 外部接口提供
void SqliteController::pre_proccess() {
	// -Create TABLE 
	createTable(CREATE_TABLE_COMPOUND, 1, CREATE_INDEX_MAXX_ON_COMPOUND);
	createTable(CREATE_TABLE_FILTER, 2, CREATE_INDEX_X_ON_FILTER, CREATE_INDEX_RANK_ON_FILTER);


	// -Fill in dates
	//pre_parsePeakDate();
	//dq_pre_buildMassHash();
	//dq_pre_buildCompound();
	//dq_pre_buildFilter();
}
bool SqliteController::initTables() {
	createTable(CREATE_TABLE_COMPOUND, 1, CREATE_INDEX_MAXX_ON_COMPOUND);
	createTable(CREATE_TABLE_FILTER, 2, CREATE_INDEX_X_ON_FILTER, CREATE_INDEX_RANK_ON_FILTER);

	return !checkConnectionError();
}
void SqliteController::createTable(const char* tableName, int indexNumber, ...) {
	
	int rc;
	sqlite3_stmt* statement;

	// TABLE
	rc = sqlite3_prepare_v2(_ppDB, tableName, -1, &statement, NULL);
	if (rc != SQLITE_OK) { std::cerr << "create" << tableName << " failed. " << std::endl; }

	rc = sqlite3_step(statement);
	if (rc != SQLITE_DONE) {
		std::cerr << "create" << tableName << " -> sqlite3_step[" << rc << "] " << sqlite3_errmsg(_ppDB) << " " << sqlite3_errcode(_ppDB) << std::endl;
	}
	sqlite3_finalize(statement);

	// INDEX
	va_list args;
	va_start(args, indexNumber);

	for (int i = 0; i < indexNumber; i++) {

		const char* indexName = va_arg(args, const char*);
		
		rc = sqlite3_prepare_v2(_ppDB, indexName, -1, &statement, NULL);
		if (rc != SQLITE_OK) { std::cerr << "create" << indexName << " failed. " << std::endl; }
		rc = sqlite3_step(statement);
		if (rc != SQLITE_DONE) {
			std::cerr << "create" << indexName << " -> sqlite3_step[" << rc << "] " << sqlite3_errmsg(_ppDB) << " " << sqlite3_errcode(_ppDB) << std::endl;
		}
		sqlite3_finalize(statement);
	}

	va_end(args);
}
// 查
void SqliteController::getPeaksByCompoundIDs(int* compoundIDs, std::vector<Peak>& peaks) {

	// 按slice单位 WHERE x=1 OR x=3 OR x=9 .. 一次性取数据
	const size_t peakSize = compoundIDs[0];
	if (peakSize <= 0)  { return; }
	//peaks.resize(peakSize);

	const int slice = 256;
	const int pages = peakSize / slice; 
	const int remain = peakSize % slice;

	char c[32];
	sqlite3_stmt *statement;
	std::string query = "SELECT CompoundID, PeakCount, PeakData FROM Compound WHERE ";

	int index = 0;
	for(int i = 1; i <= MAX_COMPOUND_ID; i++) {

		if (compoundIDs[i] != COMPOUND_FOUND && i != MAX_COMPOUND_ID) { continue; }

		if (compoundIDs[i] == COMPOUND_FOUND) {

			const int compoundID = i;
			sprintf_s(c, "CompoundID = %d ", compoundID); 
			query += c;

			index++;

			if ((index != peakSize) && (index % slice != 0) || (index == 0))  { query += "OR "; }
		}


		if ((index != 0 && index % slice == 0)||(i == MAX_COMPOUND_ID)||(index == peakSize)) {

			sqlite3_exec(_ppDB, "BEGIN;", NULL, NULL, NULL);
			sqlite3_prepare_v2(_ppDB, query.c_str(), query.size(), &statement, NULL);

			//std::cout << query.c_str() << std::endl;
			while (sqlite3_step(statement) == SQLITE_ROW) {

				Peak aPeak;
				aPeak._compoundID = sqlite3_column_int(statement, 0);
				aPeak._peakCount = sqlite3_column_int(statement, 1);
				aPeak._peakData = (const char*)sqlite3_column_text(statement, 2);
				peaks.push_back(aPeak);

			}
			sqlite3_exec(_ppDB, "COMMIT;", NULL, NULL, NULL);
			sqlite3_reset(statement);
			query = "SELECT CompoundID, PeakCount, PeakData FROM Compound WHERE ";
		}

	}
	sqlite3_finalize(statement);
}
int SqliteController::totalCompoundCounts() { 

	sqlite3_stmt* statement; 
	sqlite3_prepare_v2(_ppDB, COUNT_TOTAL_ROWS, -1, &statement, NULL);
	return query_aSingleCount(statement);
}
int SqliteController::maxCompoundID() {
	sqlite3_stmt* statement;
	sqlite3_prepare_v2(_ppDB, SELECT_MAX_COMPOUND_ID, -1, &statement, NULL);
	return query_aSingleCount(statement);
}
int SqliteController::maxPeakCount() {

	sqlite3_stmt* statement;
	sqlite3_prepare_v2(_ppDB, SELECT_MAX_PEAK_COUNT, -1, &statement, NULL);
	return query_aSingleCount(statement);
}
void SqliteController::getPeakData(int compoundID, Peak& aPeak) {

	sqlite3_stmt* statement; 
	sqlite3_prepare_v2(_ppDB, SELECT_PEAKDATA_BY_ID, -1, &statement, NULL);
	int rc = sqlite3_bind_int(statement, 1, compoundID);
	if (rc != SQLITE_OK) {
		std::cerr << "sqlite3_bind_int[" << rc << "] " << sqlite3_errmsg(_ppDB) << " " << sqlite3_errcode(_ppDB) << std::endl;
		return;
	}

	rc = sqlite3_step(statement);
	if (sqlite3_data_count(statement) > 0) {
		aPeak._peakCount = sqlite3_column_int(statement, 0);
		aPeak._peakData = (const char*)sqlite3_column_text(statement, 1);
	}
	sqlite3_reset(statement);
}
std::vector<Peak> SqliteController::getPeakDatas(int startCompoundID, int limit) {

	std::vector<Peak> peaks;
	sqlite3_stmt *statement;
	sqlite3_prepare_v2(_ppDB, SELECT_PEAKDATA_BY_RANK, -1, &statement, NULL);

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
void SqliteController::dq_getPeakDatas_v3(int* compoundIDs, std::vector<Peak>& peaks) {
	// CompoundInfo 全部读入内存，事务批量读
	const size_t peakSize = compoundIDs[0];
	peaks.resize(COUNT_COMPOUNDS);

	sqlite3_stmt *statement;
	std::string query = "SELECT PeakCount, PeakData FROM CompoundInfo ORDER BY CompoundID LIMIT ? OFFSET ?";
	sqlite3_prepare_v2(_ppDB, query.c_str(), query.size(), &statement, NULL);

	const int slice = 10000;
	const int pages = COUNT_COMPOUNDS / slice; //每次读10K行
	//const int remain = peakSize % slice;

	for (int page = 0; page != pages + 1; page++) {

		sqlite3_exec(_ppDB, "BEGIN;", NULL, NULL, NULL);
		if ((sqlite3_bind_int(statement, 1, slice) == SQLITE_OK) &&
			(sqlite3_bind_int(statement, 2, slice * page) == SQLITE_OK)) {

				while (sqlite3_step(statement) == SQLITE_ROW) {
					static int n = 0;
					peaks[n]._peakCount = sqlite3_column_int(statement, 0);
					peaks[n]._peakData = (const char*)sqlite3_column_text(statement, 1);
					n++;
				}
		}
		sqlite3_exec(_ppDB, "COMMIT;", NULL, NULL, NULL);
		sqlite3_reset(statement);
	}

	sqlite3_finalize(statement);
}
void SqliteController::getPeakPoints(int compoundID, unsigned int* x, float* y) {

	int rc;
	sqlite3_stmt *statement;

	sqlite3_exec(_ppDB, "BEGIN;", 0, 0, 0);
	rc = sqlite3_prepare_v2(_ppDB, SELECT_XY_FROM_PEAKDATA, -1, &statement, NULL);


	for (int i=1; i <= 191436; i++) { //TODO: 批量取
		if (sqlite3_bind_int(statement, 1, compoundID) == SQLITE_OK) {

			int index = 0;
			while (sqlite3_step(statement) == SQLITE_ROW) {
				unsigned int x_ = static_cast<unsigned int>(sqlite3_column_int(statement, 0));
				float y_ = static_cast<float>(sqlite3_column_int(statement, 1));

				x[index] = x_;
				y[index] = y_;

				index++;
			}
		}
	}

	sqlite3_exec(_ppDB, "COMMIT;", 0, 0, 0);
	sqlite3_finalize(statement);
}
void SqliteController::dq_getPeakPoints(std::vector<PeakPoint>& peakPoints) { //TBD
	int rc;
	sqlite3_stmt* statement;
	const std::string queryXY = "SELECT [x], [y] FROM [PeakData] WHERE CompoundID = ?";
	//const std::string queryXY = "SELECT [CompoundID] FROM [PeakData] WHERE x = ?";

	sqlite3_exec(_ppDB, "BEGIN;", 0, 0, 0);
	rc = sqlite3_prepare_v2(_ppDB, queryXY.c_str(), queryXY.size(), &statement, NULL);

	for (int i=1; i <= 200000; i++) {
		if (sqlite3_bind_int(statement, 1, i) == SQLITE_OK) {

			while (sqlite3_step(statement) == SQLITE_ROW) {

				//compoundIDs += (const char*)sqlite3_column_text(statementPeakData, 0); 
				//compoundIDs += ",";
				//count++;
			}
			sqlite3_reset(statement);
		}
	}

	sqlite3_exec(_ppDB, "COMMIT;", 0, 0, 0);
	sqlite3_finalize(statement);


}
Compound SqliteController::getCompound(int compoundID) {

	Compound aCompound;
	sqlite3_stmt *statement;
	sqlite3_prepare_v2(_ppDB, SELECT_COMPOUND_BY_ID, -1, &statement, NULL);

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
		aCompound._maxX = sqlite3_column_int(statement, 6);
		aCompound._peakData = (const char*)sqlite3_column_text(statement, 7);
	}
	sqlite3_reset(statement);
	return aCompound;
}
std::vector<Compound> SqliteController::getCompounds(int startCompoundID, int limit) {

	std::vector<Compound> compounds;
	sqlite3_stmt *statement;
	sqlite3_prepare_v2(_ppDB, SELECT_COMPOUND_BY_RANK, -1, &statement, NULL);

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
std::vector<Compound> SqliteController::getCompounds(const SearchPara& searchPara)  {
	/*
		SELECT * FROM Compound WHERE
									( CompoundID >= 31 AND CompoundID <= 34 ) 
								AND ( MassWeight >= 18 AND MassWeight <= 60 )
								AND CompoundName = 'Water' 
	*/
	std::vector<Compound> compounds;
	if (searchPara.isEmpty()) { return compounds; }

	sqlite3_stmt *statement;
	std::string query = "SELECT * FROM Compound ";
	const std::string subQuery = searchPara.parseWhereQuery();
	query += subQuery;

	int count = countCompounds(searchPara);
	if (count > 100) { query += "LIMIT 100"; }

	sqlite3_prepare_v2(_ppDB, query.c_str(), query.size(), &statement, NULL);

	while (sqlite3_step(statement) == SQLITE_ROW) {
		Compound aCompound;
		aCompound._compoundID = sqlite3_column_int(statement, 0);
		aCompound._compoundName = (const char*)sqlite3_column_text(statement, 1);
		aCompound._formula = (const char*)sqlite3_column_text(statement, 2);
		aCompound._massWeight = sqlite3_column_int(statement, 3);
		aCompound._casNo = (const char*)sqlite3_column_text(statement, 4);
		aCompound._peakCount = sqlite3_column_int(statement, 5);
		aCompound._maxX = sqlite3_column_int(statement, 6);
		aCompound._peakData = (const char*)sqlite3_column_text(statement, 7);
		compounds.push_back(aCompound);
	}

	sqlite3_reset(statement);
	return compounds;
}
int SqliteController::countCompounds(const SearchPara& searchPara) {
	int count = 0;

	if (searchPara.isEmpty()) { return count; }
	sqlite3_stmt *statement;
	std::string query = "SELECT COUNT(*) FROM Compound ";
	const std::string subQuery = searchPara.parseWhereQuery();
	query += subQuery;

	sqlite3_prepare_v2(_ppDB, query.c_str(), query.size(), &statement, NULL);
	if (sqlite3_step(statement) == SQLITE_ROW) {
		
		count = sqlite3_column_int(statement, 0);
		
	}

	sqlite3_reset(statement);
	return count;
}

// 存 / 改
void SqliteController::storePeakData(const PeakPoint& aPoint) {

	sqlite3_stmt *statement;
	sqlite3_prepare_v2(_ppDB, INSERT_PEAK_DATA, -1, &statement, NULL);

	if ((sqlite3_bind_int(statement, 1, aPoint._compoundID) == SQLITE_OK) &&
		(sqlite3_bind_int(statement, 2, aPoint._x) == SQLITE_OK) &&
		(sqlite3_bind_int(statement, 3, aPoint._y) == SQLITE_OK) ) {

			sqlite3_step(statement);
			sqlite3_reset(statement);
	}
}
void SqliteController::storeCompound(const Compound& aCompound) {

	// 写入的数据已经通过内容格式验证
	sqlite3_stmt *statement;
	sqlite3_prepare_v2(_ppDB, INSERT_COMPOUND_DATA, -1, &statement, NULL);
	
	if ((sqlite3_bind_int(statement,  1, aCompound._compoundID) == SQLITE_OK) &&
		(sqlite3_bind_text(statement, 2, aCompound._compoundName.c_str(), -1, SQLITE_STATIC) == SQLITE_OK) &&
		(sqlite3_bind_text(statement, 3, aCompound._formula.c_str(), -1, SQLITE_STATIC) == SQLITE_OK) &&
		(sqlite3_bind_int(statement,  4, aCompound._massWeight) == SQLITE_OK) &&
		(sqlite3_bind_text(statement, 5, aCompound._casNo.c_str(), -1, SQLITE_STATIC) == SQLITE_OK) &&
		(sqlite3_bind_int(statement,  6, aCompound._peakCount) == SQLITE_OK) &&
		(sqlite3_bind_int(statement,  7, aCompound._maxX) == SQLITE_OK) &&
		(sqlite3_bind_text(statement, 8, aCompound._peakData.c_str(), -1, SQLITE_STATIC) == SQLITE_OK)) {

		   sqlite3_step(statement);
		   sqlite3_reset(statement);
	}
}
void SqliteController::storeFiltePoint(const Compound& aCompound) {

	// 凡少于16个峰不收录
	const int needPeaks = 16;
	const std::string &strPeakData = aCompound._peakData;
	const int &peakCount = aCompound._peakCount;
	const int &compoundID = aCompound._compoundID;
	if (peakCount < needPeaks) { return; }

	std::vector<FilterPoint> filterPoints;

	// 分配ID号
	std::vector<FilterPoint> tmpFilterPoints;
	tmpFilterPoints.resize(peakCount);
	for (size_t j = 0; j != peakCount; j++) { tmpFilterPoints[j]._peakPoint._compoundID = compoundID; }

	// 解析获得 YrX 和 rank 数据
	pre_parsePeakDataString(strPeakData, peakCount, tmpFilterPoints);

	// 按 YrX 排序
	nth_element(tmpFilterPoints.begin(), tmpFilterPoints.begin() + needPeaks, tmpFilterPoints.end(), SqliteController::filterPointCompare_YrX); // 最大的16个 yrx
	
	// 保存 FilterPoints 点数上限
	const int filterNumbers = FILTER_PEAK_LIMIT_TIMES + 7; // 1~9
	for (int j = 0; j < filterNumbers; j++) { tmpFilterPoints[j]._rank = j + 1; } // rank 编号
	filterPoints.insert(filterPoints.end(), tmpFilterPoints.begin(), tmpFilterPoints.begin() + filterNumbers);

	// 写入数据库
	sqlite3_stmt *statement;
	sqlite3_prepare_v2(_ppDB, INSERT_FILTER_DATA, -1, &statement, NULL);

	typedef std::vector<FilterPoint>::iterator ITER;

	//sqlite3_exec(_ppDB, "BEGIN;", NULL, NULL, NULL);
	for (ITER it = filterPoints.begin(); it != filterPoints.end(); it++) {

		if ((sqlite3_bind_int(statement, 1, (*it)._peakPoint._compoundID) == SQLITE_OK) &&
			(sqlite3_bind_int(statement, 2, (*it)._peakPoint._x) == SQLITE_OK) &&
			(sqlite3_bind_int(statement, 3, (*it)._peakPoint._y) == SQLITE_OK) &&
			(sqlite3_bind_int(statement, 4, (*it)._yrx) == SQLITE_OK) &&
			(sqlite3_bind_int(statement, 5, (*it)._rank) == SQLITE_OK)) {

				sqlite3_step(statement);
				sqlite3_reset(statement);
		}
	}

	//sqlite3_exec(_ppDB, "COMMIT;", 0, 0, 0);
	
}

// 删
void SqliteController::deleteCompound(const int compoundID) {
	sqlite3_stmt *statement;
	sqlite3_prepare_v2(_ppDB, DELETE_COMPOUND_BY_ID, -1, &statement, NULL);

	if (sqlite3_bind_int(statement,  1, compoundID) == SQLITE_OK) {
		sqlite3_step(statement);
		sqlite3_reset(statement);
	}

	// 同时删除关联索引表内的数据
	sqlite3_prepare_v2(_ppDB, DELETE_FILTER_BY_ID, -1, &statement, NULL);

	if (sqlite3_bind_int(statement,  1, compoundID) == SQLITE_OK) {
		sqlite3_step(statement);
		sqlite3_reset(statement);
	}
}
// - 内部接口
std::wstring SqliteController::acsii2wideByte(std::string& strascii) {  
	int widesize = MultiByteToWideChar (CP_ACP, 0, (char*)strascii.c_str(), -1, NULL, 0);  
	if (widesize == ERROR_NO_UNICODE_TRANSLATION) {  
		throw std::exception("Invalid UTF-8 sequence.");  
	}  
	if (widesize == 0) {  
		throw std::exception("Error in conversion.");  
	}  

	std::vector<wchar_t> resultstring(widesize);  
	int convresult = MultiByteToWideChar (CP_ACP, 0, (char*)strascii.c_str(), -1, &resultstring[0], widesize);  

	if (convresult != widesize) {  
		throw std::exception("La falla!");  
	}  

	return std::wstring(&resultstring[0]);  
}  
std::string SqliteController::unicode2utf8(const std::wstring& wString) {
	int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, wString.c_str(), -1, NULL, 0, NULL, NULL);  
	if (utf8size == 0) {  
		throw std::exception("Error in conversion.");  
	}  

	std::vector<char> resultstring(utf8size);  
	int convresult = ::WideCharToMultiByte(CP_UTF8, 0, wString.c_str(), -1, &resultstring[0], utf8size, NULL, NULL);  

	if (convresult != utf8size) {  
		throw std::exception("La falla!");  
	}  

	return std::string(&resultstring[0]); 
}
std::string SqliteController::ascii2utf8(std::string& strAsciiCode) {
	std::string strRet("");  
	std::wstring wstr = acsii2wideByte(strAsciiCode);  
	strRet = unicode2utf8(wstr);  
	return strRet;  
}

void SqliteController::queryCompoundData(std::vector<Compound> &selectedCompounds) {

	if (!selectedCompounds.empty()) { selectedCompounds.clear(); }

	//TODO: 实际检索数据代替随机测试数据
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
		y[index] = atoi(strY.c_str());

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
void SqliteController::pre_parsePeakDataString(const std::string& strPeakData, int peakCount, std::vector<FilterPoint> &filterPoints) {
	std::string::size_type i = 0;
	std::string::size_type j = strPeakData.find(';');

	for (int index = 0; index < peakCount; index++) {

		std::string strXY = strPeakData.substr(i, j-i);
		std::string::size_type w = strXY.find(' ');
		std::string strX = strXY.substr(0, w);
		std::string strY = strXY.substr(w+1, strXY.length());

		FilterPoint aFilterPoint;
		int x_ = atoi(strX.c_str());
		int y_ = atoi(strY.c_str());
		int yrx_ = static_cast<int>(sqrt(x_ * 1.0f) * y_);
 
		filterPoints[index]._peakPoint._x = x_;
		filterPoints[index]._peakPoint._y = y_;
		filterPoints[index]._yrx = yrx_;
		//filterPoints.push_back(aFilterPoint);

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

		Peak aPeak;
		getPeakData(compoundID, aPeak);
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
void SqliteController::parseCompoundIDs(const std::string &strCompoundIDs, int* compoundIDs) {

	std::string::size_type i = 0;
	std::string::size_type j = strCompoundIDs.find(',');

	while ( j != std::string::npos) {

		std::string strCompoundID = strCompoundIDs.substr(i, j-i);
		int compoundID = atoi(strCompoundID.c_str());

		if (compoundIDs[compoundID] == 0) {
			compoundIDs[compoundID] = 1;
		}
		if (compoundIDs[compoundID] == 1) {
			compoundIDs[compoundID] = 2;
			compoundIDs[0]++;
		}
			
		i = ++j;
		j = strCompoundIDs.find(',', j);
	}
}
void SqliteController::parseCompound(Compound& aCompound, unsigned int *x, float *y) {

	const std::string strPeakData = aCompound._peakData;
	const int peakCount = aCompound._peakCount;

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

	// 直接获取最大质量数
	aCompound._maxX = x[peakCount-1];
}
void SqliteController::parsePeakData(const std::string& strPeakData, int peakCount, unsigned int *x, float *y) {
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
void SqliteController::dq_pre_buildMassHash() {

	sqlite3_stmt* statement;
	
	sqlite3_prepare_v2(_ppDB, COUNT_MASS_ROWS, -1, &statement, NULL);
	int massCount = 0;
	if (sqlite3_step(statement) == SQLITE_ROW) {
		massCount = sqlite3_column_int(statement, 0);
	}
	sqlite3_finalize(statement);

	// 按升序填充 Mass 序号
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

	// 填充 IDs 	
	
	sqlite3_stmt* statementPeakData;
	const std::string queryPeakData = "SELECT [compoundID] FROM [PeakData] WHERE x = ?";
	
	sqlite3_stmt* statementMassHash;
	const std::string insertMassHash = "INSERT OR REPLACE INTO [MassHash](Mass, IDs) VALUES (?, ?)";
	
	sqlite3_exec(_ppDB, "BEGIN;", 0, 0, 0);

	for (int massIndex = 1; massIndex <= MAX_MASS; massIndex++) {

		// 读取PeakData 中的对应关系
		int count = 0;
		double timeStart = (double)clock();

		std::string compoundIDs = "";
		sqlite3_prepare_v2(_ppDB, queryPeakData.c_str(), queryPeakData.size(), &statementPeakData, NULL);
		if (sqlite3_bind_int(statementPeakData, 1, massIndex) == SQLITE_OK) {

			while (sqlite3_step(statementPeakData) == SQLITE_ROW) {

				compoundIDs += (const char*)sqlite3_column_text(statementPeakData, 0); 
				compoundIDs += ",";
				count++;
			}
		}
		
		sqlite3_finalize(statementPeakData);

		// 存入MassHash
		sqlite3_prepare_v2(_ppDB, insertMassHash.c_str(), insertMassHash.size(), &statementMassHash, NULL);
		if ((sqlite3_bind_int(statementMassHash, 1, massIndex) == SQLITE_OK) &&
			(sqlite3_bind_text(statementMassHash, 2, compoundIDs.c_str(), -1, SQLITE_STATIC) == SQLITE_OK) ) {

				sqlite3_step(statementMassHash);
				sqlite3_finalize(statementMassHash);
		}

		double timeFinish = (double)clock();
		std::cout << massIndex << " done ("<< count <<") "<< MAX_MASS - massIndex << " to go. =" << timeFinish-timeStart <<" ms" << std::endl;
	}

	sqlite3_exec(_ppDB, "COMMIT;", 0, 0, 0);
}
void SqliteController::dq_pre_getCompounds(std::vector<Compound> &compounds) {

	sqlite3_stmt *statement;
	std::string query = "SELECT * FROM [Compound] ORDER BY CompoundID LIMIT ? OFFSET ?";
	sqlite3_prepare_v2(_ppDB, query.c_str(), query.size(), &statement, NULL);

	const int slice = 10000;
	const int pages = COUNT_COMPOUNDS / slice;
	for (int page = 0; page != pages + 1; page++) {

		sqlite3_exec(_ppDB, "BEGIN;", NULL, NULL, NULL);
		if ((sqlite3_bind_int(statement, 1, slice) == SQLITE_OK) &&
			(sqlite3_bind_int(statement, 2, slice * page) == SQLITE_OK)) {

				while (sqlite3_step(statement) == SQLITE_ROW) {
					Compound aCompound;
					aCompound._compoundID = sqlite3_column_int(statement, 0);
					aCompound._compoundName = (const char*)sqlite3_column_text(statement, 1);
					aCompound._formula = (const char*)sqlite3_column_text(statement, 2);
					aCompound._massWeight = sqlite3_column_int(statement, 3);
					aCompound._casNo = (const char*)sqlite3_column_text(statement, 4);
					aCompound._peakCount = sqlite3_column_int(statement, 5);
					aCompound._maxX = sqlite3_column_int(statement, 6);
					aCompound._peakData = (const char*)sqlite3_column_text(statement, 7); 

					compounds.push_back(aCompound);
				}
		}

		sqlite3_exec(_ppDB, "COMMIT;", 0, 0, 0);
		sqlite3_reset(statement);
	}
}
void SqliteController::dq_pre_buildCompound(std::vector<Compound> &compounds) {

	sqlite3_stmt *statement;
	std::string query = "INSERT OR REPLACE INTO Compound "
			" ([CompoundID], [CompoundName], [Formula], [MassWeight], [CasNo], [PeakCount], [MaxX], [PeakData])"
			" VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
	sqlite3_prepare_v2(_ppDB, query.c_str(), query.size(), &statement, NULL);

	const size_t compoundSize = compounds.size();
	if (compoundSize != COUNT_COMPOUNDS - 2) { std::cout << "Compound Size goes wrong" << std::endl; }

	sqlite3_exec(_ppDB, "BEGIN;", NULL, NULL, NULL);
	for (size_t i = 0; i != compoundSize; i++) {
		if ((sqlite3_bind_int(statement,  1, compounds[i]._compoundID) == SQLITE_OK) &&
			(sqlite3_bind_text(statement, 2, compounds[i]._compoundName.c_str(), compounds[i]._compoundName.size()-1, SQLITE_STATIC) == SQLITE_OK) &&
			(sqlite3_bind_text(statement, 3, compounds[i]._formula.c_str(), compounds[i]._formula.size()-1, SQLITE_STATIC) == SQLITE_OK) &&
			(sqlite3_bind_int(statement,  4, compounds[i]._massWeight) == SQLITE_OK) &&
			(sqlite3_bind_text(statement, 5, compounds[i]._casNo.c_str(), -1, SQLITE_STATIC) == SQLITE_OK) &&
			(sqlite3_bind_int(statement,  6, compounds[i]._peakCount) == SQLITE_OK) &&
			(sqlite3_bind_int(statement,  7, compounds[i]._maxX) == SQLITE_OK) &&
			(sqlite3_bind_text(statement, 8, compounds[i]._peakData.c_str(), -1, SQLITE_STATIC) == SQLITE_OK)) {

				sqlite3_step(statement);
				sqlite3_reset(statement);
		}
	}
	sqlite3_exec(_ppDB, "COMMIT;", 0, 0, 0);
	
}
void SqliteController::dq_pre_buildFilter() {
	// 取出20w记录，逐一解析，处理YrX, 排序前16数据 另插入maxX行 到Filter Table
	std::vector<Compound> compounds;
	dq_pre_getCompounds(compounds);

	std::vector<FilterPoint> filterPoints;

	const size_t compoundSize = compounds.size();
	for (size_t i = 0; i != compoundSize; i++) {

		const std::string &strPeakData = compounds[i]._peakData;
		const int &peakCount = compounds[i]._peakCount;
		const int &compoundID = compounds[i]._compoundID;

		std::vector<FilterPoint> tmpFilterPoints;
		tmpFilterPoints.resize(peakCount);
		for (size_t j=0; j!=peakCount; j++) { tmpFilterPoints[j]._peakPoint._compoundID = compoundID; }

		pre_parsePeakDataString(strPeakData, peakCount, tmpFilterPoints);


		// 凡少于16个峰，（3819个）不收录
		const int needPeaks = 16;
		if (peakCount >= needPeaks) {
			nth_element(tmpFilterPoints.begin(), tmpFilterPoints.begin() + needPeaks, tmpFilterPoints.end(), SqliteController::filterPointCompare_YrX); // 最大的16个 yrx
			for (int j = 0; j != 9; j++) { tmpFilterPoints[j]._rank = j + 1; } // rank 编号
			filterPoints.insert(filterPoints.end(), tmpFilterPoints.begin(), tmpFilterPoints.begin() + 9);
		} 
		
	}

	// Filter 表 3214346 行数据
	sqlite3_stmt *statement;
	sqlite3_prepare_v2(_ppDB, INSERT_FILTER_DATA, -1, &statement, NULL);

	typedef std::vector<FilterPoint>::iterator ITER;
	//const size_t filterPointsSize = filterPoints.size();

	sqlite3_exec(_ppDB, "BEGIN;", NULL, NULL, NULL);
	for (ITER it = filterPoints.begin(); it != filterPoints.end(); it++) {

		if ((sqlite3_bind_int(statement, 1, (*it)._peakPoint._compoundID) == SQLITE_OK) &&
			(sqlite3_bind_int(statement, 2, (*it)._peakPoint._x) == SQLITE_OK) &&
			(sqlite3_bind_int(statement, 3, (*it)._peakPoint._y) == SQLITE_OK) &&
			(sqlite3_bind_int(statement, 4, (*it)._yrx) == SQLITE_OK) &&
			(sqlite3_bind_int(statement, 5, (*it)._rank) == SQLITE_OK)) {

				sqlite3_step(statement);
				sqlite3_reset(statement);
		}
	}

	sqlite3_exec(_ppDB, "COMMIT;", 0, 0, 0);
}
// - 过滤
void SqliteController::dq_filterPeakByTwoMass(const Compound &aCompound, int* compoundIDs) {

	const std::string strPeakData = aCompound._peakData;
	const int peakCount = aCompound._peakCount;
	
	// 解析字符串 找出最大和次最大的Y值对应的X
	std::vector<PeakPoint> peakPoints;
	pre_parsePeakDataString(strPeakData, peakCount, peakPoints);
	if (peakPoints.size() < 2) { return; }

	nth_element(peakPoints.begin(), peakPoints.begin() + 2, peakPoints.end(), SqliteController::peakPointCompare_Y); // 最大的2个数
	const int maxXSubXSize = 2;
	int maxXSubX[maxXSubXSize] = { peakPoints[0]._x, peakPoints[1]._x };

	sqlite3_stmt* statement;
	const std::string query = "SELECT IDs FROM MassHash WHERE Mass = ? LIMIT 1;"; 
	sqlite3_prepare_v2(_ppDB, query.c_str(), query.size(), &statement, NULL);

	// 选用std::set<int> 速度太慢且会在后期的造成 5s 的析构时间消耗
	// 直接开一个内存存放所有情况 //compoundIDs = new int[COUNT_COMPOUNDS];

	for (int i = 0; i != maxXSubXSize; i++) {
		if ((sqlite3_bind_int(statement, 1, maxXSubX[i]) == SQLITE_OK)) {
			if (sqlite3_step(statement) == SQLITE_ROW) {
				const std::string strCompoundIDs = (const char*)sqlite3_column_text(statement, 0);
				parseCompoundIDs(strCompoundIDs, compoundIDs);
				sqlite3_reset(statement);
			}
		}
	}
	sqlite3_finalize(statement);
}
void SqliteController::dq_filterPeakByMaxX(const int maxX, int* compoundIDs) {

	sqlite3_stmt* statement;
	const std::string query = "SELECT CompoundID FROM Compound WHERE MaxX = ?;"; 
	sqlite3_prepare_v2(_ppDB, query.c_str(), query.size(), &statement, NULL);

	compoundIDs[0] = 0;
	if ((sqlite3_bind_int(statement, 1, maxX) == SQLITE_OK)) {
		while (sqlite3_step(statement) == SQLITE_ROW) {
			int compoundID = sqlite3_column_int(statement, 0);

			if (compoundIDs[compoundID] == 2) {
				compoundIDs[compoundID] = 3;
				compoundIDs[0]++;
			}
		}
	}
}
void SqliteController::dq_filterPeakBy14(const std::vector<FilterPoint> &filterPoints, int* compoundIDs) {
	// 按未知物质 14 个最高Y对应 X 在 Filter 取对应存在 14 个 X 的 CompoundID

	sqlite3_stmt* statement;
	char c[16];
	std::string query = "SELECT [CompoundID] FROM Filter WHERE ";

	const size_t pointSize = filterPoints.size();
	for (size_t i = 0; i != pointSize; i++) {
		const int X = filterPoints[i]._peakPoint._x;
		sprintf_s(c, "X = %d ", X); 
		query += c;
		if (i != pointSize - 1)  { query += "OR "; }	
	}
	//std::cout << query.c_str() << std::endl;

	sqlite3_prepare_v2(_ppDB, query.c_str(), query.size(), &statement, NULL);
	while (sqlite3_step(statement) == SQLITE_ROW) {

		int compoundID = sqlite3_column_int(statement, 0);
		compoundIDs[compoundID]++;

	}
	sqlite3_finalize(statement);

	for (int i = 1; i != COUNT_COMPOUNDS; i++) {
		if (compoundIDs[i] == 14) { 
			compoundIDs[i] = COMPOUND_FOUND;
			compoundIDs[0]++;
			//std::cout << i << " Found" << std::endl;
		}
	}
	//std::cout << compoundIDs[0] << " TOGO" << std::endl;
}
void SqliteController::_filterPeakBy08(const std::vector<FilterPoint> &filterPoints, int* compoundIDs) {
	// 按未知物质 1-8 个最高Y对应 X 在 是否处于 Filter 取对应存在 8-16 个YrX最大对应的 X 的范围内
	 
	sqlite3_stmt* statement;
	const std::string query = "SELECT CompoundID FROM Filter WHERE  (X = ?) AND (Rank >= ? AND Rank <= ?);"; 
	sqlite3_prepare_v2(_ppDB, query.c_str(), query.size(), &statement, NULL);

	//const int filterPeakLimitNumbers = 2; // 过滤轮数
	

	for (int i = 0; i != FILTER_PEAK_LIMIT_TIMES; i++) {

		int filterTime = i + 1; //筛选的轮数
		int filterLower = 1;
		int filterUpper = filterTime + 7; // 若第1次，峰个数为1-8， 第N次，峰个数为1-N+7
		int findCount = 0;
		const int X = filterPoints[i]._peakPoint._x;
		int countFound = 0;

		if ((sqlite3_bind_int(statement, 1, X) == SQLITE_OK) && 
			(sqlite3_bind_int(statement, 2, filterLower) == SQLITE_OK) && 
			(sqlite3_bind_int(statement, 3, filterUpper) == SQLITE_OK)) {

				while (sqlite3_step(statement) == SQLITE_ROW) {

					// 直接取出来的CompoundID，不重复
					int compoundID = sqlite3_column_int(statement, 0);
					compoundIDs[compoundID]++;
					countFound++;
				}
		}

		sqlite3_reset(statement);
		//std::cout << countFound << " Found" << std::endl;
	}

	sqlite3_finalize(statement);

	//
	compoundIDs[0] = 0;
	for (int i = 0; i < MAX_COMPOUND_ID; i++) {
		if (compoundIDs[i] == FILTER_PEAK_LIMIT_TIMES) {
			compoundIDs[i] = COMPOUND_FOUND;
			//std::cout << i << std::endl;
			compoundIDs[0] += 1;
		}
		
	}
	//std::cout << compoundIDs[0] << "\t Found. ";
}
void SqliteController::filterCompounds_C(const Compound& testCompound, int *compoundIDs) {

	const int peakCount = testCompound._peakCount;
	if (peakCount < FILTER_PEAK_LIMIT_TIMES) { return; }

	const int filterPeakLimitNumbers = FILTER_PEAK_LIMIT_TIMES; 
	std::string strPeakData = testCompound._peakData;
	std::vector<FilterPoint> unknownPeakPoints; 
	unknownPeakPoints.resize(peakCount);

	pre_parsePeakDataString(strPeakData, peakCount, unknownPeakPoints);
	nth_element(unknownPeakPoints.begin(), 
		unknownPeakPoints.begin() + filterPeakLimitNumbers, unknownPeakPoints.end(), 
		SqliteController::filterPointCompare_YrX);

	std::vector<FilterPoint> nthPoints;
	nthPoints.insert(nthPoints.end(), unknownPeakPoints.begin(), unknownPeakPoints.begin() + filterPeakLimitNumbers);
	
	_filterPeakBy08(nthPoints, compoundIDs);

	//dq_filterPeakBy14(nthPoints, compoundIDs);
}
void SqliteController::filterCompounds_D(const Compound& testCompound, int *compoundIDs) {

	const int peakCount = testCompound._peakCount;
	const int filterPeakLimitNumbers = 14; 
	std::string strPeakData = testCompound._peakData;
	std::vector<FilterPoint> unknownPeakPoints; 
	unknownPeakPoints.resize(peakCount);

	pre_parsePeakDataString(strPeakData, peakCount, unknownPeakPoints);
	nth_element(unknownPeakPoints.begin(), 
		unknownPeakPoints.begin() + filterPeakLimitNumbers, unknownPeakPoints.end(), 
		SqliteController::filterPointCompare_YrX);

	std::vector<FilterPoint> nthPoints;
	nthPoints.insert(nthPoints.end(), unknownPeakPoints.begin(), unknownPeakPoints.begin() + filterPeakLimitNumbers);

	//_filterPeakBy08(nthPoints, compoundIDs);

	dq_filterPeakBy14(nthPoints, compoundIDs);
}
void SqliteController::filterCompounds(const Compound& testCompound, int *compoundIDs) {
/*
	【X】筛选方式 TypeA（testCompound峰数目≤5，原始数据 X，Y）
	1未知谱图中强度最大的5个峰（对应的X）要与谱库中谱图强度最大的5个峰（对应的X）相同
	2未知谱图中X的最大值要与谱库中谱图X的最大值相同
	--SELECT * FROM FilterXY WHERE Y=? OR Y=? OR Y=? OR Y=? OR Y=? ;

	【X】筛选方式 TypeB（testCompound峰数目≥6，原始数据 X，Y）
	未知谱图中强度最大的6个峰（对应的X）要与谱库中谱图强度最大的6个峰（对应的X）相同
	--SELECT * FROM FilterXY WHERE Y=? OR Y=? OR Y=? OR Y=? OR Y=? OR Y=? ;

	【√】筛选方式 TypeC（testCompound峰数目≥8，处理后的数据 X，Y×√X）
	未知谱图中Y√X强度第1的峰（对应的X）要出现在谱库中谱图强度最大的8个峰（对应的X）范围内包含相等，进行2轮
	--SELECT CompoundID, X, Rank FROM Filter WHERE  (X = ?) AND (Rank >= ? AND Rank <= ?) ;

	【X】筛选方式 TypeD（testCompound峰数目≥14，处理后的数据集合 X，Y×√X）
	未知谱图中强度最大的14个峰（对应的X）要与谱库中谱图强度最大的14个峰（对应的X）相同
	--SELECT [CompoundID] FROM Filter WHERE X=%d OR … X=%d
*/

	filterCompounds_C(testCompound, compoundIDs);

	//const int peakCount = testCompound._peakCount;
	//int *B_IDs = new int[MAX_COMPOUND_ID + 1](); 
	//int *C_IDs = new int[MAX_COMPOUND_ID + 1](); 
	//int *D_IDs = new int[MAX_COMPOUND_ID + 1](); 

	//double timeStart = (double)clock();
	//if (peakCount <= 5) { }
	//if (peakCount >= 6) { }
	//if (peakCount >= 8) { filterCompounds_C(testCompound, C_IDs); }
	//if (peakCount >= 14) { filterCompounds_D(testCompound, D_IDs); }

	//for (int i = 1; i <= MAX_COMPOUND_ID; i++) {
	//	if (C_IDs[i] == COMPOUND_FOUND || D_IDs[i] == COMPOUND_FOUND) {
	//		compoundIDs[i] = COMPOUND_FOUND;
	//		compoundIDs[0]++;
	//	}
	//}

	//double timeFinish = (double)clock();
	//std::cout << "FilterCompouns:\t" << timeFinish - timeStart << std::endl;
	//std::cout << testCompound._compoundID;
	//std::cout << "\tC-" << C_IDs[0] << "\tD-" << D_IDs[0];
	//std::cout << "\tALL--" << compoundIDs[0] << std::endl;

	//delete [] D_IDs;
	//delete [] C_IDs;
	//delete [] B_IDs;
}

