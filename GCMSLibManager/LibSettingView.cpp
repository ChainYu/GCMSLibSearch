// LibSettingView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LibSettingView.h"
#include "afxdialogex.h"
#include "SqliteController.h"


// LibSettingView �Ի���

IMPLEMENT_DYNAMIC(LibSettingView, CDialogEx)

LibSettingView::LibSettingView(CWnd* pParent /*=NULL*/)
	: CDialogEx(LibSettingView::IDD, pParent)
{
	_defaultDBPath = CString(_T("E:\\GCMSLibSearch\\GCMSLibSearch\\nist.db"));
	_cstrDBPath = _defaultDBPath;
}

LibSettingView::~LibSettingView()
{
}

BOOL LibSettingView::OnInitDialog() {
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_EDIT_DB_PATH)->SetWindowText(_defaultDBPath);
	return TRUE;
}

void LibSettingView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void LibSettingView::setCompoundsOnEditCtrls(const Compound &aCompound) {
	CString cstrMassWeight;
	cstrMassWeight.Format(_T("%d"), aCompound._massWeight);

	GetDlgItem(IDC_EDIT_COMPOUND_NAME)->SetWindowText(CString(aCompound._compoundName.c_str()));
	GetDlgItem(IDC_EDIT_FORMULA)->SetWindowText(CString(aCompound._formula.c_str()));
	GetDlgItem(IDC_EDIT_MASSWEIGHT)->SetWindowText(cstrMassWeight);
	GetDlgItem(IDC_EDIT_CASNO)->SetWindowText(CString(aCompound._casNo.c_str()));
	GetDlgItem(IDC_EDIT_PEAKDATA)->SetWindowText(CString(aCompound._peakData.c_str()));
}
void LibSettingView::getCompoundsOnEditCtrls(Compound &aCompound) {
	CString cstrName, cstrFormula, cstrMass, cstrCas, cstrPeakData;

	GetDlgItem(IDC_EDIT_COMPOUND_NAME)->GetWindowText(cstrName);
	GetDlgItem(IDC_EDIT_FORMULA)->GetWindowText(cstrFormula);
	GetDlgItem(IDC_EDIT_MASSWEIGHT)->GetWindowText(cstrMass);
	GetDlgItem(IDC_EDIT_CASNO)->GetWindowText(cstrCas);
	GetDlgItem(IDC_EDIT_PEAKDATA)->GetWindowText(cstrPeakData);

	aCompound._compoundName = CT2A(cstrName);
	aCompound._formula = CT2A(cstrFormula);
	aCompound._massWeight = _ttoi(cstrMass);
	aCompound._casNo = CT2A(cstrCas);
	aCompound._peakData = CT2A(cstrPeakData);
}


BEGIN_MESSAGE_MAP(LibSettingView, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE_DB, &LibSettingView::OnBnClickedChooseDB)
	ON_BN_CLICKED(IDC_BUTTON_QUERY_COMPOUND, &LibSettingView::OnBnClickedQueryCompound)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_DB, &LibSettingView::OnBnClickedCreateDB)
END_MESSAGE_MAP()


// LibSettingView ��Ϣ�������


void LibSettingView::OnBnClickedChooseDB() {
	CString strPath = _T("");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("�׿����ݿ� (*.db)|*.db|All Files (*.*)|*.*||"), NULL);

	if (fileDlg.DoModal() == IDOK) {

		strPath = fileDlg.GetPathName();
		GetDlgItem(IDC_EDIT_DB_PATH)->SetWindowText(strPath);
		_cstrDBPath = strPath;
	} else { 

		GetDlgItem(IDC_EDIT_DB_PATH)->SetWindowText(_cstrDBPath);
		return; 
	}
}


void LibSettingView::OnBnClickedQueryCompound() {
	// ���õ� CompoundID
	CString cstrCompoundID;
	GetDlgItem(IDC_EDIT_COMPOUND_ID)->GetWindowText(cstrCompoundID);
	int compoundID = _ttoi(cstrCompoundID);
	if (compoundID <= 0) {
		::MessageBox(NULL, _T("ָ���Ļ�����ID��Χ�������0"), _T("����"), MB_OK | MB_ICONWARNING);
		return;
	}
	// �ӵ�ǰ�׿�����
	std::string sqlitePath = CT2A(_cstrDBPath);
	SqliteController sqliteController(sqlitePath);
	Compound compound = sqliteController.getCompound(compoundID);

	// ��ʾ���
	setCompoundsOnEditCtrls(compound);
}


void LibSettingView::OnBnClickedCreateDB() {

	CString cstrPath = _T("");
	CFileDialog fileDlg(FALSE, _T("db"), NULL, OFN_HIDEREADONLY, _T("�׿����ݿ� (*.db)|*.db||"), NULL);

	if (fileDlg.DoModal() == IDOK) {
		cstrPath = fileDlg.GetPathName();
	} else { return; }

	// TODO: ����Ƿ�������ͻ
	
	HANDLE hFile;
	hFile = CreateFile(cstrPath, GENERIC_WRITE,FILE_SHARE_WRITE , NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		::MessageBox(NULL, _T("�½�������� ʧ��"), _T("����"), MB_OK | MB_ICONWARNING);
		return;
	}
	CloseHandle(hFile);

	const std::string strPath = CT2A(cstrPath);
	SqliteController sqliteController(strPath);
	
	// �����±�
	bool success = sqliteController.initTables();
	if (success) {
		CString dbName = fileDlg.GetFileName();
		CString msgText = _T("�½�������� ") + dbName + _T(" �ɹ�");
		::MessageBox(NULL, msgText, _T("֪ͨ"), MB_OK);

		GetDlgItem(IDC_EDIT_DB_PATH)->SetWindowText(cstrPath);
		_cstrDBPath = cstrPath;

	} else {
		::MessageBox(NULL, _T("�½�������� ʧ��"), _T("����"), MB_OK | MB_ICONWARNING);

		GetDlgItem(IDC_EDIT_DB_PATH)->SetWindowText(_T(""));
		_cstrDBPath = _T("");
	}
}
