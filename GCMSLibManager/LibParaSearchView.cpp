// LibParaSearchView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LibParaSearchView.h"
#include "afxdialogex.h"
#include "SqliteController.h"


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

BOOL LibParaSearchView::OnInitDialog() {
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_EDIT_SEARCH_DB_PATH)->SetWindowText(_defaultDBPath);

	GetDlgItem(IDC_EDIT_COMPOUND_NAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_CAS_NO)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_COMPOUND_FORMULA)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MASS_LOWER)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MASS_UPPER)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ID_LOWER)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ID_UPPER)->EnableWindow(FALSE);

	std::string sqlitePath = CT2A(_defaultDBPath);
	SqliteController sqliteController(sqlitePath);
	_maxCompoundID = sqliteController.maxCompoundID();

	return TRUE;
}
void LibParaSearchView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

int LibParaSearchView::checkSearchPara(const SearchPara& searchPara) {

	// ��� MASS �� ID �������� (��ֵ�����
	if (searchPara._massLower != "" && searchPara._massUpper != "") {
		int massLower = atoi( searchPara._massLower.c_str() );
		int massUpper = atoi( searchPara._massUpper.c_str() );
		if (massLower < 1) { return CHECK_MASS_LOWER_FAIL; }
		if (massUpper > 9999) { return CHECK_MASS_UPPER_FAIL; }
	}

	if (searchPara._idLower != "" && searchPara._idUpper != "") {
		int idLower = atoi( searchPara._idLower.c_str() );
		int idUpper = atoi( searchPara._idUpper.c_str() );
		if (idLower < 1) { return CHECK_ID_LOWER_FAIL; }
		if (idUpper > _maxCompoundID) { return CHECK_ID_UPPER_FAIL; }
	}
	  
	return CHECK_PASS;
}
void LibParaSearchView::getSearchPara(SearchPara& searchPara) {
	CString cstrName, cstrCAS, cstrFormula;
	CString cstrMassLower, cstrMassUpper;
	CString cstrIDLower, cstrIDUpper;

	if ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_COMPOUND_NAME) ) {
		GetDlgItem(IDC_EDIT_COMPOUND_NAME)->GetWindowText(cstrName);
	}
	if ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_CAS_NO) ) {
		GetDlgItem(IDC_EDIT_CAS_NO)->GetWindowText(cstrCAS);
	}
	if ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_FORMULA) ) {
		GetDlgItem(IDC_EDIT_COMPOUND_FORMULA)->GetWindowText(cstrFormula);
	}
	if ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_MASS_RANGE) ) {
		GetDlgItem(IDC_EDIT_MASS_LOWER)->GetWindowText(cstrMassLower);
		GetDlgItem(IDC_EDIT_MASS_UPPER)->GetWindowText(cstrMassUpper);
	}
	if ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_COMPOUND_ID_RANGE) ) {
		GetDlgItem(IDC_EDIT_ID_LOWER)->GetWindowText(cstrIDLower);
		GetDlgItem(IDC_EDIT_ID_UPPER)->GetWindowText(cstrIDUpper);
	}

	searchPara._name = CT2A(cstrName);
	searchPara._cas = CT2A(cstrCAS);
	searchPara._formula = CT2A(cstrFormula);
	searchPara._massLower = CT2A(cstrMassLower);
	searchPara._massUpper = CT2A(cstrMassUpper);
	searchPara._idLower = CT2A(cstrIDLower);
	searchPara._idUpper = CT2A(cstrIDUpper);	
}



BEGIN_MESSAGE_MAP(LibParaSearchView, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_COMPOUND_NAME, &LibParaSearchView::OnBnClickedCheckCompoundName)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH_DB_PATH, &LibParaSearchView::OnBnClickedButtonSearchDBPath)
	ON_BN_CLICKED(IDC_CHECK_CAS_NO, &LibParaSearchView::OnBnClickedCheckCasNo)
	ON_BN_CLICKED(IDC_CHECK_FORMULA, &LibParaSearchView::OnBnClickedCheckFormula)
	ON_BN_CLICKED(IDC_CHECK_MASS_RANGE, &LibParaSearchView::OnBnClickedCheckMassRange)
	ON_BN_CLICKED(IDC_CHECK_COMPOUND_ID_RANGE, &LibParaSearchView::OnBnClickedCheckCompoundIdRange)
	ON_BN_CLICKED(IDC_BUTTON_LIB_PARA_SEARCH, &LibParaSearchView::OnBnClickedButtonLibParaSearch)
END_MESSAGE_MAP()




// LibParaSearchView ��Ϣ�������
void LibParaSearchView::OnBnClickedButtonSearchDBPath() {
	CString strPath = _T("");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("�׿����ݿ� (*.db)|*.db|All Files (*.*)|*.*||"), NULL);

	if (fileDlg.DoModal() == IDOK) {

		strPath = fileDlg.GetPathName();
		GetDlgItem(IDC_EDIT_SEARCH_DB_PATH)->SetWindowText(strPath);
		_currentDBPath = strPath;
	} else { 

		GetDlgItem(IDC_EDIT_SEARCH_DB_PATH)->SetWindowText(_currentDBPath);
		return; 
	}

	std::string sqlitePath = CT2A(_currentDBPath);
	SqliteController sqliteController(sqlitePath);
	_maxCompoundID = sqliteController.maxCompoundID();
}

// - CheckBox ��ѡ��
void LibParaSearchView::OnBnClickedCheckCompoundName() {
	BOOL isReadOnly = ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_COMPOUND_NAME) );
	GetDlgItem(IDC_EDIT_COMPOUND_NAME)->EnableWindow(isReadOnly);
}
void LibParaSearchView::OnBnClickedCheckCasNo() {
	BOOL isReadOnly = ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_CAS_NO) );
	GetDlgItem(IDC_EDIT_CAS_NO)->EnableWindow(isReadOnly);
}
void LibParaSearchView::OnBnClickedCheckFormula() {
	BOOL isReadOnly = ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_FORMULA) );
	GetDlgItem(IDC_EDIT_COMPOUND_FORMULA)->EnableWindow(isReadOnly);
}
void LibParaSearchView::OnBnClickedCheckMassRange() {
	BOOL isReadOnly = ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_MASS_RANGE) );
	GetDlgItem(IDC_EDIT_MASS_LOWER)->EnableWindow(isReadOnly);
	GetDlgItem(IDC_EDIT_MASS_UPPER)->EnableWindow(isReadOnly);
}
void LibParaSearchView::OnBnClickedCheckCompoundIdRange() {
	BOOL isReadOnly = ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_COMPOUND_ID_RANGE) );
	GetDlgItem(IDC_EDIT_ID_LOWER)->EnableWindow(isReadOnly);
	GetDlgItem(IDC_EDIT_ID_UPPER)->EnableWindow(isReadOnly);
}
// - ����
void LibParaSearchView::OnBnClickedButtonLibParaSearch() {
	// ���ͨ����֤�ļ�������
	SearchPara searchPara;
	getSearchPara(searchPara);

	// ��֤��������
	int ret = checkSearchPara(searchPara);
	if (ret == CHECK_MASS_LOWER_FAIL || ret == CHECK_MASS_UPPER_FAIL) {
		::MessageBox(NULL, _T("����ķ����������޶���Χ"), _T("֪ͨ"), MB_OK);
		return;
	}

	if (ret == CHECK_ID_LOWER_FAIL || ret == CHECK_ID_UPPER_FAIL) {
		::MessageBox(NULL, _T("����Ļ�����ID�����޶���Χ"), _T("֪ͨ"), MB_OK);
		return;
	}

	// �õ����� ȫ����������
	
	std::string sqlitePath = CT2A(_currentDBPath);
	SqliteController sqliteController(sqlitePath);
	std::vector<Compound> compounds = sqliteController.getCompounds(searchPara);
}
