#pragma once

#include "GCMSLibManager.h"

#define CHECK_PASS					0
#define CHECK_MASS_LOWER_FAIL		1
#define CHECK_MASS_UPPER_FAIL		2
#define CHECK_ID_LOWER_FAIL			3
#define CHECK_ID_UPPER_FAIL			4

// LibParaSearchView �Ի���

class LibParaSearchView : public CDialogEx
{
	DECLARE_DYNAMIC(LibParaSearchView)

public:
	LibParaSearchView(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~LibParaSearchView();

// �Ի�������
	enum { IDD = IDD_LIBPARASEARCHVIEW };

	afx_msg void OnBnClickedCheckCompoundName();
	afx_msg void OnBnClickedButtonSearchDBPath();
	afx_msg void OnBnClickedCheckCasNo();
	afx_msg void OnBnClickedCheckFormula();
	afx_msg void OnBnClickedCheckMassRange();
	afx_msg void OnBnClickedCheckCompoundIdRange();
	afx_msg void OnBnClickedButtonLibParaSearch();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	void LibParaSearchView::getSearchPara(SearchPara& searchPara);
	int LibParaSearchView::checkSearchPara(const SearchPara& searchPara);

public:
	CString _defaultDBPath; // ϵͳĬ���׿�
	CString _currentDBPath; // ��ǰʹ���׿�

	int _maxCompoundID;
};
