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
	: CDialogEx(CSearchResultView::IDD, pParent)
{

}

CSearchResultView::~CSearchResultView() {
	if(_pInstance) {
		delete _pInstance;
		_pInstance = NULL;
	}
}

BOOL CSearchResultView::OnInitDialog() {
	CDialogEx::OnInitDialog();
	initListCtrl();
	return TRUE;
}

void CSearchResultView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMPOUND_LIST, _compoundList);
}


BEGIN_MESSAGE_MAP(CSearchResultView, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CSearchResultView::OnBnClickedCancel)
	ON_NOTIFY(NM_CLICK, IDC_COMPOUND_LIST, &CSearchResultView::OnNMClickCompoundList)
END_MESSAGE_MAP()


void CSearchResultView::initListCtrl() {
	_compoundList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	//_compoundList.SetBkColor(RGB(205,205,205));
	//_compoundList.SetTextBkColor(RGB(205,205,205));
	_compoundList.SetTextColor(RGB(45,45,45));

	_compoundList.InsertColumn(0, _T("No."), LVCFMT_LEFT,50);
	_compoundList.InsertColumn(1, _T("����"), LVCFMT_LEFT,510);
	_compoundList.InsertColumn(2, _T("����ʽ"), LVCFMT_LEFT,110);
	_compoundList.InsertColumn(3, _T("������"), LVCFMT_LEFT,50);
	_compoundList.InsertColumn(4, _T("CAS��"), LVCFMT_LEFT,100); 
	_compoundList.InsertColumn(5, _T("ƥ���"), LVCFMT_LEFT,50);
}

void CSearchResultView::clearList() {
	_compoundList.DeleteAllItems();
}


void CSearchResultView::fillCompoundList(const std::vector<Compound> &compounds) {

	clearList();
	_compounds = compounds;

	const int listRows = compounds.size();
	const int listCols = 5;
	for (int row = 0; row < listRows; row++) {


		CString strCompound[listCols];
		strCompound[0].Format(_T("%d"), compounds[row]._compoundID);
		strCompound[1] = compounds[row]._compoundName.c_str();
		strCompound[2] = compounds[row]._formula.c_str();
		strCompound[3].Format(_T("%d"), compounds[row]._massWeight);
		strCompound[4] = compounds[row]._casNo.c_str();

		_compoundList.InsertItem(row, strCompound[0], row);
		for (int col = 1; col < listCols; col++) {
			_compoundList.SetItemText(row, col, strCompound[col]);
		}
	}
}

// CSearchResultView ��Ϣ�������
void CSearchResultView::OnBnClickedCancel() {
	CDialog::OnCancel();
}


void CSearchResultView::OnNMClickCompoundList(NMHDR *pNMHDR, LRESULT *pResult) {

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	 


	*pResult = 0;
}
