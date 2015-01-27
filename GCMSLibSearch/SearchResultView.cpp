// SearchResultView.cpp : ʵ���ļ�

#include "stdafx.h"
#include "GCMSLibSearch.h"
#include "SearchResultView.h"
#include "afxdialogex.h"

CSearchResultView* CSearchResultView::_pInstance = NULL;

// CSearchResultView �Ի���
IMPLEMENT_DYNAMIC(CSearchResultView, CDialog)

CSearchResultView* CSearchResultView::getInstance() {
	if (_pInstance == NULL) {
		_pInstance = new CSearchResultView;
		_pInstance->Create(IDD_SEARCH_RESULT_VIEW, NULL);
	}
	return _pInstance;
}

CSearchResultView::CSearchResultView(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchResultView::IDD, pParent)
{

}

CSearchResultView::~CSearchResultView() {
	if(_pInstance) {
		delete _pInstance;
		_pInstance = NULL;
	}
}

void CSearchResultView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSearchResultView, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CSearchResultView::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSearchResultView ��Ϣ�������


void CSearchResultView::OnBnClickedCancel()
{
	CDialog::OnCancel();
	//DestroyWindow();
}
