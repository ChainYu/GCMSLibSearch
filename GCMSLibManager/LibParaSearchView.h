#pragma once

#include "GCMSLibManager.h"
#include <sstream>
#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
	( std::ostringstream() << std::dec << x ) ).str()

#define CHECK_PASS					0
#define CHECK_EMPTY					1
#define CHECK_MASS_RANGE_FAIL		2
#define CHECK_ID_RANGE_FAIL			3


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

	void LibParaSearchView::setSearchPara();
	void LibParaSearchView::getSearchPara();
	int LibParaSearchView::checkSearchPara();

public:
	CString _defaultDBPath; // ϵͳĬ���׿�
	CString _currentDBPath; // ��ǰʹ���׿�

	SearchPara _searchPara;
	int _maxCompoundID;
	
};
