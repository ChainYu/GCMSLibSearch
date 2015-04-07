#pragma once

#include "GCMSLibManager.h"

// LibSearchStrategyView �Ի���

class LibSearchStrategyView : public CDialogEx
{
	DECLARE_DYNAMIC(LibSearchStrategyView)
	enum { IDD = IDD_LIBSEARCHSTRATEGYVIEW };  

	afx_msg void OnBnClickedChoosePath1();
	afx_msg void OnBnClickedChoosePath2();
	afx_msg void OnBnClickedChoosePath3();
	afx_msg void OnBnClickedOk();

public:
	LibSearchStrategyView(CWnd* pParent, LibConfig& libConfig);   // ��׼���캯��
	virtual ~LibSearchStrategyView();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	CString getFilePath();
	void initCtrls();

public:
	LibConfig _libConfig;

};
