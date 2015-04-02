// LibParaSearchView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LibParaSearchView.h"
#include "afxdialogex.h"


// LibParaSearchView �Ի���

IMPLEMENT_DYNAMIC(LibParaSearchView, CDialogEx)

LibParaSearchView::LibParaSearchView(CWnd* pParent /*=NULL*/)
	: CDialogEx(LibParaSearchView::IDD, pParent)
{
	_defaultDBPath = CString(_T("E:\\GCMSLibSearch\\GCMSLibSearch\\nist.db")); // TODO: ��MD5���ļ�
	_currentDBPath = _defaultDBPath;
}

LibParaSearchView::~LibParaSearchView()
{
}

void LibParaSearchView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(LibParaSearchView, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_COMPOUND_NAME, &LibParaSearchView::OnBnClickedCheckCompoundName)
END_MESSAGE_MAP()


// LibParaSearchView ��Ϣ�������

// - CheckBox ��ѡ��
void LibParaSearchView::OnBnClickedCheckCompoundName() {

	if ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_COMPOUND_NAME) ) {
		GetDlgItem(IDC_EDIT_COMPOUND_NAME)->EnableWindow(TRUE);
	} else {
		GetDlgItem(IDC_EDIT_COMPOUND_NAME)->EnableWindow(FALSE);
	}
	
}
