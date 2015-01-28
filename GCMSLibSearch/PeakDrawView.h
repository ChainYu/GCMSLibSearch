#pragma once
#include "SuperChartLib.h"
#include <iostream>
#include <vector>

class CPeakDrawView {

public:
	CPeakDrawView(void);
	CPeakDrawView(CSuperChartCtrl* pSuperChartCtrl);
	virtual ~CPeakDrawView(void);

public:
	VOID setChartCtrl(CSuperChartCtrl* upChartCtrl, CSuperChartCtrl* downChartCtrl);
	VOID drawPeak(const CString &strPeakData);  //��ʾ��ͼ (��)

	VOID dropPeakCompare();
	VOID drawPeakCompare(const CString &strPeakData); //��ʾ��ͼ (��)

private:
	void parsePeakData(const CString &strPeakData, std::vector<std::pair<int, int> > &peakData); // TODO�� �������ɻ���

private:
	CSuperChartCtrl* _pPeakChart;    //�ϣ�����ͼ
	CSuperChartCtrl* _pCompareChart; //�£��Ա�ͼ

	//CChartLineSerie* _pPeakLine; //��ͼ
	
	


};

