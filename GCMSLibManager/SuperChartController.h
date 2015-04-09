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
	VOID drawCompoundString(std::vector<CPoint> &peakPoints); //����������Yֵ��ӦXֵ

	VOID dropCompoundChart();

private:
	void parsePeakData(const CString &strPeakData, std::vector<CPoint> &peakPoints);

private:
	CSuperChartCtrl* _pChart;
};




	