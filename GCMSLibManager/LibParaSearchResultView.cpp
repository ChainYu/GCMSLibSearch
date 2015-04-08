// LibParaSearchResultView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "LibParaSearchResultView.h"



// LibParaSearchResultView �Ի���

IMPLEMENT_DYNAMIC(LibParaSearchResultView, CDialogEx)

LibParaSearchResultView::LibParaSearchResultView(const std::vector<Compound> &compounds, CWnd* pParent /*=NULL*/)
	: CDialogEx(LibParaSearchResultView::IDD, pParent)
{
	_compounds = compounds;
}

LibParaSearchResultView::~LibParaSearchResultView()
{
}

void LibParaSearchResultView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SEARCH_RESULT, _compoundsList);
}

BOOL LibParaSearchResultView::OnInitDialog(){
	CDialogEx::OnInitDialog();
	initListCtrl();
	fillCompoundList();
	return TRUE;
}


BEGIN_MESSAGE_MAP(LibParaSearchResultView, CDialogEx)
	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, &LibParaSearchResultView::OnHdnItemdblclickListSearchResult)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SEARCH_RESULT, &LibParaSearchResultView::OnNMDblclkListSearchResult)
END_MESSAGE_MAP()

VOID LibParaSearchResultView::initListCtrl() {
	_compoundsList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	_compoundsList.SetTextColor(RGB(45,45,45));

	_compoundsList.InsertColumn(0, _T("ID."), LVCFMT_LEFT,50);
	_compoundsList.InsertColumn(1, _T("����������"), LVCFMT_LEFT,220);
	_compoundsList.InsertColumn(2, _T("����ʽ"), LVCFMT_LEFT,110);
	_compoundsList.InsertColumn(3, _T("������"), LVCFMT_LEFT,50);
	_compoundsList.InsertColumn(4, _T("CAS��"), LVCFMT_LEFT,100); 
}

VOID LibParaSearchResultView::fillCompoundList() {

	//clearList();
	//_compounds = compounds;

	const int listRows = _compounds.size();
	const int listCols = 5;
	for (int row = 0; row < listRows; row++) {


		CString strCompound[listCols];
		strCompound[0].Format(_T("%d"), _compounds[row]._compoundID);
		strCompound[1] = _compounds[row]._compoundName.c_str();
		strCompound[2] = _compounds[row]._formula.c_str();
		strCompound[3].Format(_T("%d"), _compounds[row]._massWeight);
		strCompound[4] = _compounds[row]._casNo.c_str();

		_compoundsList.InsertItem(row, strCompound[0], row);
		for (int col = 1; col < listCols; col++) {
			_compoundsList.SetItemText(row, col, strCompound[col]);
		}
	}
}
// LibParaSearchResultView ��Ϣ�������


void LibParaSearchResultView::OnHdnItemdblclickListSearchResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	
	*pResult = 0;
}


void LibParaSearchResultView::OnNMDblclkListSearchResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	UpdateData(TRUE);

	POSITION pos = _compoundsList.GetFirstSelectedItemPosition();
	if (pos) {
		//pos = _compoundList.GetSelectedItemPosition();
		int nItem = _compoundsList.GetSelectionMark();
		CString strCompoundID = _compoundsList.GetItemText(nItem, 0);
		int compoundID = _ttoi(strCompoundID);

		// ��� _compounds �ж�Ӧ iD ���±�
		for (int i = 0; i != _compounds.size(); i++) {
			if (_compounds[i]._compoundID == compoundID) {

				// ��ʾ��ͼ
				CompoundChartView compoundChartView(_compounds[i]); //�������ͼ
				compoundChartView.DoModal();

				break;
			}
		}
	}
	UpdateData(FALSE);
	*pResult = 0;
}
