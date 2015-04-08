#pragma once

#include "GCMSLibManager.h"
#include "Compound.h"
#include "CompoundChartView.h"

#include <vector>
// LibParaSearchResultView �Ի���

class LibParaSearchResultView : public CDialogEx
{
	DECLARE_DYNAMIC(LibParaSearchResultView)

public:
	LibParaSearchResultView(const std::vector<Compound> &compounds, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~LibParaSearchResultView();

	

// �Ի�������
	enum { IDD = IDD_LIBPARASEARCHRESULTVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	
	DECLARE_MESSAGE_MAP()
	afx_msg void OnHdnItemdblclickListSearchResult(NMHDR *pNMHDR, LRESULT *pResult);

	BOOL LibParaSearchResultView::OnInitDialog();
	VOID LibParaSearchResultView::initListCtrl();
	VOID LibParaSearchResultView::fillCompoundList();

public:
	CListCtrl _compoundsList;
	std::vector<Compound> _compounds;
	afx_msg void OnNMDblclkListSearchResult(NMHDR *pNMHDR, LRESULT *pResult);
};
