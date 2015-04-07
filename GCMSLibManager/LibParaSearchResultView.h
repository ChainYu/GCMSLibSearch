#pragma once

#include "GCMSLibManager.h"
#include <vector>
#include "Compound.h"
// LibParaSearchResultView �Ի���

class LibParaSearchResultView : public CDialogEx
{
	DECLARE_DYNAMIC(LibParaSearchResultView)

public:
	LibParaSearchResultView(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~LibParaSearchResultView();

	void LibParaSearchResultView::fillCompoundList(const std::vector<Compound> &compounds);

// �Ի�������
	enum { IDD = IDD_LIBPARASEARCHRESULTVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	BOOL LibParaSearchResultView::OnInitDialog();
	DECLARE_MESSAGE_MAP()

	VOID LibParaSearchResultView::initListCtrl();

public:
	CListCtrl _compoundsList;
	std::vector<Compound> _compounds;
};
