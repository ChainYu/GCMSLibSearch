// LibParaSearchView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LibParaSearchView.h"
#include "afxdialogex.h"
#include "SqliteController.h"




// LibParaSearchView �Ի���

IMPLEMENT_DYNAMIC(LibParaSearchView, CDialogEx)

LibParaSearchView::LibParaSearchView(const CString defaultLibPathName, CWnd* pParent /*=NULL*/)
	: CDialogEx(LibParaSearchView::IDD, pParent)
{
	_defaultDBPath = defaultLibPathName; // CString(_T("E:\\GCMSLibSearch\\GCMSLibSearch\\nist.db")); // TODO: ��MD5���ļ�
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
void LibParaSearchView::setSearchPara() {

	CString cstrName = CString(_searchPara._name.c_str());
	CString cstrCAS = CString(_searchPara._cas.c_str());
	CString cstrFormula = CString(_searchPara._formula.c_str());
	CString cstrMassLower = CString(_searchPara._massLower.c_str());
	CString cstrMassUpper = CString(_searchPara._massUpper.c_str());
	CString cstrIDLower = CString(_searchPara._idLower.c_str());
	CString cstrIDUpper = CString(_searchPara._idUpper.c_str());

	if ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_COMPOUND_NAME) ) {
		GetDlgItem(IDC_EDIT_COMPOUND_NAME)->SetWindowText(cstrName);
	}
	if ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_CAS_NO) ) {
		GetDlgItem(IDC_EDIT_CAS_NO)->SetWindowText(cstrCAS);
	}
	if ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_FORMULA) ) {
		GetDlgItem(IDC_EDIT_COMPOUND_FORMULA)->SetWindowText(cstrFormula);
	}
	if ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_MASS_RANGE) ) {
		GetDlgItem(IDC_EDIT_MASS_LOWER)->SetWindowText(cstrMassLower);
		GetDlgItem(IDC_EDIT_MASS_UPPER)->SetWindowText(cstrMassUpper);
	}
	if ( BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_COMPOUND_ID_RANGE) ) {
		GetDlgItem(IDC_EDIT_ID_LOWER)->SetWindowText(cstrIDLower);
		GetDlgItem(IDC_EDIT_ID_UPPER)->SetWindowText(cstrIDUpper);
	}
}
void LibParaSearchView::getSearchPara() {
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

	_searchPara._name = CT2A(cstrName);
	_searchPara._cas = CT2A(cstrCAS);
	_searchPara._formula = CT2A(cstrFormula);
	_searchPara._massLower = CT2A(cstrMassLower);
	_searchPara._massUpper = CT2A(cstrMassUpper);
	_searchPara._idLower = CT2A(cstrIDLower);
	_searchPara._idUpper = CT2A(cstrIDUpper);	
}
int LibParaSearchView::checkSearchPara() {

	getSearchPara();

	if (_searchPara.isEmpty()) {
		return CHECK_EMPTY;
	}

	// ��� MASS �� ID �������� (��ֵ�����
	if (_searchPara._massLower != "" || _searchPara._massUpper != "") {
		int massLower = atoi( _searchPara._massLower.c_str() );
		int massUpper = atoi( _searchPara._massUpper.c_str() );
		if (massLower < 1) { return CHECK_MASS_RANGE_FAIL; }
		if (massUpper > 9999) { return CHECK_MASS_RANGE_FAIL; }
		if (massLower > massUpper) { return CHECK_MASS_RANGE_FAIL; }

		_searchPara._massLower = SSTR(massLower);
		_searchPara._massUpper = SSTR(massUpper);
	}

	if (_searchPara._idLower != "" || _searchPara._idUpper != "") {
		int idLower = atoi( _searchPara._idLower.c_str() );
		int idUpper = atoi( _searchPara._idUpper.c_str() );
		if (idLower < 1) { return CHECK_ID_RANGE_FAIL; }
		if (idUpper > _maxCompoundID) { return CHECK_ID_RANGE_FAIL; }
		if (idLower > idUpper) { return CHECK_ID_RANGE_FAIL; }

		_searchPara._idLower = SSTR(idLower); 
		_searchPara._idUpper = SSTR(idUpper);
	}

	// TODO�����С��,������ŵ�


	setSearchPara();
	return CHECK_PASS;
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

	// ��֤��ǰ��������
	int ret = checkSearchPara();
	if (ret == CHECK_EMPTY) {
		::MessageBox(NULL, _T("��������һ�������������"), _T("֪ͨ"), MB_OK);
		return;
	}
	if (ret == CHECK_MASS_RANGE_FAIL) {
		::MessageBox(NULL, _T("����ķ����������޶���Χ"), _T("֪ͨ"), MB_OK);
		return;
	}

	if (ret == CHECK_ID_RANGE_FAIL) {
		::MessageBox(NULL, _T("����Ļ�����ID�����޶���Χ"), _T("֪ͨ"), MB_OK);
		return;
	}

	// �õ����� ȫ����������
	std::string sqlitePath = CT2A(_currentDBPath);
	SqliteController sqliteController(sqlitePath);
	
	// �ж����������Ŀ��
	int count = sqliteController.countCompounds(_searchPara);
	if (count > 100) {
		CString strCount;
		strCount.Format(_T("%d"), count);
		CString strMsg = _T("������ ") + strCount + _T(" ������������ֻ��ʾǰ100������");
		::MessageBox(NULL, strMsg, _T("֪ͨ"), MB_OK);
	}
	// ������
	std::vector<Compound> compounds = sqliteController.getCompounds(_searchPara);

	LibParaSearchResultView libParaSearchResultView(compounds);
	libParaSearchResultView.DoModal();
	
}
