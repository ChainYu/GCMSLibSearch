#pragma once

#include "GCMSLibManager.h"
// LibParaSearchView �Ի���

class LibParaSearchView : public CDialogEx
{
	DECLARE_DYNAMIC(LibParaSearchView)

public:
	LibParaSearchView(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~LibParaSearchView();

// �Ի�������
	enum { IDD = IDD_LIBPARASEARCHVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedCheckCompoundName();


public:
	CString _defaultDBPath; // ϵͳĬ���׿�
	CString _currentDBPath; // ��ǰʹ���׿�
	
	afx_msg void OnBnClickedButtonSearchDBPath();
	afx_msg void OnBnClickedCheckCasNo();
	afx_msg void OnBnClickedCheckFormula();
	afx_msg void OnBnClickedCheckMassRange();
	afx_msg void OnBnClickedCheckCompoundIdRange();
	afx_msg void OnBnClickedButtonLibParaSearch();
};
