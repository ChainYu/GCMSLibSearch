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
	VOID setChartCtrl(CSuperChartCtrl* pSuperChartCtrl);
	VOID drawPeak(const CString &strPeakData);  //��ʾ��ͼ

private:
	void parsePeakData(const CString &strPeakData, std::vector<std::pair<int, int> > &peakData); // TODO�� �������ɻ���

private:
	CSuperChartCtrl* _pPeakChart; 
	CChartLineSerie* _pPeakLine; //��ͼ
	
	


};

