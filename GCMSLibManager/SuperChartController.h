#pragma once
#include "SuperChartLib.h"
#include <vector>


class SuperChartController
{
public:
	SuperChartController(void);
	SuperChartController(CSuperChartCtrl* pSuperChartCtrl);
	~SuperChartController(void);

	VOID setChartCtrl(CSuperChartCtrl* pSuperChartCtrl);
	VOID drawCompoundChart(const CString &strPeakData);  //��ʾ��ͼ 
	VOID dropCompoundChart();

private:
	void parsePeakData(const CString &strPeakData, std::vector<std::pair<int, int> > &peakData); // TODO�� �������ɻ���

private:
	CSuperChartCtrl* _pChart;
};




	