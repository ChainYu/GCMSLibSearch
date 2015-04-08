// CompoundChartView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CompoundChartView.h"
#include "afxdialogex.h"


// CompoundChartView �Ի���

IMPLEMENT_DYNAMIC(CompoundChartView, CDialogEx)


CompoundChartView::CompoundChartView(const Compound& compound, CWnd* pParent /*=NULL*/)
	: CDialogEx(CompoundChartView::IDD, pParent) {
	_compound = compound;
}

CompoundChartView::~CompoundChartView()
{
}

void CompoundChartView::init() {
	CRect rcCompoundChart;
	GetDlgItem(IDC_EDIT_COMPOUND_CHART)->GetWindowRect(rcCompoundChart);
	ScreenToClient(rcCompoundChart);

	_compoundChart.Create(this, rcCompoundChart, 0, WS_CHILD|WS_VISIBLE);
	_superChartController.setChartCtrl(&_compoundChart);

	// ���÷�ͼ
	const CString cstrPeakData = CString(_compound._peakData.c_str());
	_superChartController.drawCompoundChart(cstrPeakData);
}

BOOL CompoundChartView::OnInitDialog() {
	CDialogEx::OnInitDialog();
	init();
	return TRUE;

}

void CompoundChartView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CompoundChartView, CDialogEx)
END_MESSAGE_MAP()


// CompoundChartView ��Ϣ�������
