#pragma once

#include "GCMSLibManager.h"

// CompoundChartView �Ի���

class CompoundChartView : public CDialogEx
{
	DECLARE_DYNAMIC(CompoundChartView)

public:
	CompoundChartView(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CompoundChartView();

// �Ի�������
	enum { IDD = IDD_COMPOUNDCHARTVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	BOOL CompoundChartView::OnInitDialog();
	VOID CompoundChartView::init();
	DECLARE_MESSAGE_MAP()

private:
	CSuperChartCtrl _compoundChart; 
};
