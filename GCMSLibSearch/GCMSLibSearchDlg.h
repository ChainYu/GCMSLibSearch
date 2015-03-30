// GCMSLibSearchDlg.h : ͷ�ļ�
#pragma once

#include "PeakDrawView.h"
#include "..\GCMSLibManager\Compound.h"
#include "SearchResultView.h"


// CGCMSLibSearchDlg �Ի���
class CGCMSLibSearchDlg : public CDialogEx
{
// ����
public:
	CGCMSLibSearchDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_GCMSLIBSEARCH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	void init();


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnCancel();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


private:
	CSuperChartCtrl _peakChart; 
	CSuperChartCtrl _compareChart;

	CPeakDrawView _peakDrawView;
	

public:
	afx_msg void OnBnLoadTestingPeakData();
	afx_msg void OnBnShowSearchResult();

	void drawPeakCompare(const CString &strPeakData);
	afx_msg void OnBnClickedCallDLL();
	afx_msg void OnBnClickedLibSetting();
	afx_msg void OnBnClickedParaSearch();
};
