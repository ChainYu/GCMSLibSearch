#pragma once
#include "afxcmn.h"
#include "..\GCMSLibManager\Compound.h"
#include "GCMSLibSearchDlg.h"

#include <vector>

class CGCMSLibSearchDlg;

// CSearchResultView �Ի���

class CSearchResultView : public CDialogEx
{
//����ģʽ
protected:
	static CSearchResultView* _pInstance;
public:
	static CSearchResultView* getInstance();
	

	DECLARE_DYNAMIC(CSearchResultView)
	enum { IDD = IDD_SEARCH_RESULT_VIEW };

protected:
	CSearchResultView(CWnd* pParent = NULL);
	virtual ~CSearchResultView();

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	
	DECLARE_MESSAGE_MAP()

private:
	void initListCtrl();
	void clearList();

	CListCtrl _compoundList;
	std::vector<Compound> _compounds;
	CGCMSLibSearchDlg* _pGCMSLibSearchDlg; //TODO: ���ɹ۲���ģʽ

public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNMClickCompoundList(NMHDR *pNMHDR, LRESULT *pResult);
	void fillCompoundList(const std::vector<Compound> &compounds);
	void setNofityObject(CGCMSLibSearchDlg* pDlg); //TODO: ���ɹ۲���ģʽ

	

};



