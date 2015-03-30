#pragma once

#include "GCMSLibManager.h"
// LibSettingView �Ի���

class LibSettingView : public CDialogEx
{
	DECLARE_DYNAMIC(LibSettingView)

public:
	LibSettingView(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~LibSettingView();

// �Ի�������
	enum { IDD = IDD_LIBSETTINGVIEW };

	afx_msg void OnBnClickedChooseDB();
	afx_msg void OnBnClickedQueryCompound();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	void setCompoundsOnEditCtrls(const Compound &aCompound);
	void getCompoundsOnEditCtrls(Compound &aCompound);

public:
	CString _defaultDBPath;
	CString _cstrDBPath;


	
	afx_msg void OnBnClickedCreateDB();
};
