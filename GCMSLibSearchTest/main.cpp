#include <iostream>
#include <time.h>
#include "../GCMSLibSearch/Compound.h"
#include "../GCMSLibSearch/SqliteController.h"
#include "../GCMSLibSearch/match.h"

void parsePeakData(const std::string& strPeakData, int peakCount, unsigned int *x, float *y) {
	
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

void test_totalCompoundCounts(SqliteController *pSqlController) {
	std::cout << pSqlController->totalCompoundCounts();
}
void test_getCompounds(SqliteController *pSqlController) {
	for (int page = 0; page < 300; page++) {
		int limit = 5;
		std::vector<Compound> compounds = pSqlController->getCompounds(123450 + page * limit, limit);

		for (size_t i = 0; i < compounds.size(); i++) {
			compounds[i].printBrief();
		}
		std::cout << "--- P" << page << "----------------" << std::endl; 
	}
}
void test_storeCompound(SqliteController *pSqlController) {
	Compound aCompound = pSqlController->getCompound(2);
	aCompound.print();
	std::cout << "---------------------" << std::endl; 
	aCompound._compoundID = 2;
	aCompound._compoundName = "CHain";
	aCompound._formula = "xxx";
	aCompound._massWeight = 233;
	aCompound._casNo = "7782-39-0";
	aCompound._peakCount = 3;
	aCompound._peakData = "2 55;4 222;5 111;";

	pSqlController->storeCompound(aCompound);
	aCompound = pSqlController->getCompound(2);
	aCompound.print();
}
void test_diffSpectrum(SqliteController *pSqlController, int times) {
/************************************************************************/
/* ���ܣ���ͼ�Ա�
   ������
   Mass_Match����ƥ��������[����]
   AjustedAbundance_Match����ƥ�����������[����]
   MassCount_Match����ƥ������

   Mass_Lib������ͼ������[����]
   AjustedAbundance_Lib������ͼ���������[����]
   MassCount_Lib������ͼ�����
   ����ֵ��
   -1��ƥ��ʧ��
   ˵���������Mass_Match��Mass_Lib��Ҫ����������С�������С�
*/
/************************************************************************/
	Compound testCompound = pSqlController->getCompound(4);

	const int matchPeakCount = testCompound._peakCount;
	unsigned int* matchMass = new unsigned int[matchPeakCount];
	float* matchAbundance = new float[matchPeakCount];
	const std::string strPeakData = testCompound._peakData;
	parsePeakData(strPeakData, matchPeakCount, matchMass, matchAbundance);

	// test -10000 times
	const int limit = 100;
	const int pages = times / limit;
	for (int page = 0; page < pages; page++) {
		
		std::vector<Compound> compounds = pSqlController->getCompounds(1 + page * limit, limit);

		for (size_t i = 0; i < compounds.size(); i++) {
			const int libPeakCount = compounds[i]._peakCount;
			unsigned int* libMass = new unsigned int[libPeakCount];
			float* libAbundance = new float[libPeakCount];
			const std::string strLibPeakData = compounds[i]._peakData;
			parsePeakData(strLibPeakData, libPeakCount, libMass, libAbundance);

			unsigned int matchedDegree = DiffSpectrum(matchMass, matchAbundance, matchPeakCount,
														libMass, libAbundance, libPeakCount);

			//std::cout << "[" << compounds[i]._compoundID << "]: " << matchedDegree << std::endl;


			delete [] libMass;
			delete [] libAbundance;
		}
	}

	delete [] matchMass;
	delete [] matchAbundance;
}

int main() {
	
	SqliteController sqlController("../ms.db");

	double timeStart = (double)clock(); 
	//test_diffSpectrum(&sqlController, 10000); // 4200ms
	test_diffSpectrum(&sqlController, 100000); // 293-297s -> 279-325s
	double timeFinish = (double)clock(); //����ʱ��
	
	std::cout << "run time: " << (timeFinish - timeStart) << std::endl;
	system("PAUSE");
	return 0;
}