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

}

LibSettingView::~LibSettingView()
{
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
END_MESSAGE_MAP()


// LibSettingView ��Ϣ�������


void LibSettingView::OnBnClickedChooseDB() {
	CString strPath = _T("");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("�׿����ݿ� (*.db)|*.db|All Files (*.*)|*.*||"), NULL);

	if (fileDlg.DoModal()) {
		strPath = fileDlg.GetPathName();
	}
	
	GetDlgItem(IDC_EDIT_DB_PATH)->SetWindowText(strPath);
	_cstrDBPath = strPath;
}


void LibSettingView::OnBnClickedQueryCompound() {
	// ���õ� CompoundID
	CString cstrCompoundID;
	GetDlgItem(IDC_EDIT_COMPOUND_ID)->GetWindowText(cstrCompoundID);
	int compoundID = _ttoi(cstrCompoundID);
	if (compoundID <= 0) {
		::MessageBox(NULL, _T("ָ���Ļ�����ID��Χ�������0"), _T("����"), MB_OK | MB_ICONWARNING);
	}
	// �ӵ�ǰ�׿�����
	std::string sqlitePath = CT2A(_cstrDBPath);
	SqliteController sqliteController(sqlitePath);
	Compound compound = sqliteController.getCompound(compoundID);

	// ��ʾ���
	setCompoundsOnEditCtrls(compound);

}
