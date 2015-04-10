#include "StdAfx.h"
#include "SuperChartController.h"


SuperChartController::SuperChartController(void) {
	_pChart = NULL;
}


SuperChartController::~SuperChartController(void)
{
}


VOID SuperChartController::setChartCtrl(CSuperChartCtrl* pSuperChartCtrl) {

	if ( (NULL != pSuperChartCtrl) && (_pChart != pSuperChartCtrl) ) {
		_pChart = pSuperChartCtrl;
	}
}

void SuperChartController::parsePeakData(const CString &strPeakData, std::vector<CPoint> &peakPoints) {
	// ��֤ xn > .. > x2 > x1 �������

	if (!peakPoints.empty()) { peakPoints.clear(); }

	int index = 0;
	while (true) {

		CString strXY;
		AfxExtractSubString(strXY, (LPCTSTR)strPeakData, index, ';');
		if (strXY == _T("")) { break; }

		CString strX, strY;
		AfxExtractSubString(strX, (LPCTSTR)strXY, 0, ' ');
		AfxExtractSubString(strY, (LPCTSTR)strXY, 1, ' ');

		peakPoints.push_back(CPoint(_ttoi(strX), _ttoi(strY)));
		index++;
	}
}

VOID SuperChartController::drawCompoundChart(const CString &strPeakData) {
	// �Դ��������_T("12 108;13 229;14 999;15 22;26 18;27 58;28 178;29 23;40 18;41 108;42 431;43 8;") �ַ���������ʾ

	std::vector<CPoint> peakPoints;
	parsePeakData(strPeakData, peakPoints);

	_pChart->EnableRefresh(false);

	//
	drawCompoundString(peakPoints);

	double min_x = 0.0;
	double max_x = 0.0;
	double min_y = 0.0;
	double max_y = 0.0;

	// ���Ʒ�ͼ
	for (int i=0; i != peakPoints.size(); i++) {

		CChartLineSerie* line;  
		line = _pChart->CreateLineSerie();
		line->SetColor(RGB(210, 0, 0));
		line->m_vPoints.InitPoints(4);

		double dx[] = { (double)peakPoints[i].x, (double)peakPoints[i].x };
		double dy[] = {  0.0f, (double)peakPoints[i].y };

		line->m_vPoints.AddPoints(dx, dy, 2);

		if (peakPoints[i].x > max_x) { max_x = peakPoints[i].x; }
		if (peakPoints[i].y > max_y) { max_y = peakPoints[i].y; }
	}

	CChartAxis* pAxisSelect;
	_pChart->SetBackColor(RGB(235, 235, 235)); //���ñ�����ɫ

	pAxisSelect = _pChart->GetLeftAxis(); //����Y����
	if (pAxisSelect == NULL) return;
	pAxisSelect->SetAutomatic(false);
	pAxisSelect->SetCoordinate(min_y, max_y * 1.1, COORDINATE_SET);

	pAxisSelect = _pChart->GetBottomAxis(); //����X����
	if (pAxisSelect == NULL) return;
	pAxisSelect->SetAutomatic(false);
	pAxisSelect->SetCoordinate(min_x, max_x * 1.1, COORDINATE_SET);

	pAxisSelect->SetVisible(true);
	_pChart->EnableRefresh(true);
	_pChart->RefreshCtrl();	

	
}

VOID SuperChartController::drawCompoundString(std::vector<CPoint> &peakPoints) {
	// �õ����Xֵ�󣬷ָ��10�����䣬ÿ������ѡ������Yֵ���룬����Y�������޾���MaxY��10%
	const int peakPointsSize = peakPoints.size();
	int maxX = peakPoints[peakPointsSize - 1].x;
	int maxY = 0;
	for (int i = 0; i != peakPointsSize; i++) {
		if (peakPoints[i].y > maxY) {
			maxY = peakPoints[i].y;
		}
	}

	int interval = maxX / 20;
	int intervalIndex = 0;
	int intervalX = 0;
	int intervalY = 0;
	for (int i = 0; i != peakPointsSize; i++) {

		int xLower = interval * intervalIndex;
		int xUpper = interval + xLower;

		int x = peakPoints[i].x;
		int y = peakPoints[i].y;

		if (xLower <= x && x <= xUpper) {
			if (y > intervalY) { intervalY = y; intervalX = x; }
		}

		if (x > xUpper) {

			if (intervalY > (maxY * 0.1)) {
				CString strMark;
				strMark.Format(_T("%d"), intervalX);
				_pChart->AddChartString(intervalX * 0.9, intervalY * 1.15, strMark);
			}


			intervalIndex += 1; 
			intervalY = 0;

		}
	}

	_pChart->DisplayAllChartStrings();

}