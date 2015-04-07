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
	_defaultDBPath = CString(_T("E:\\GCMSLibSearch\\GCMSLibSearch\\nist.db")); // TODO: ��MD5���ļ�
	_currentDBPath = _defaultDBPath;
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

	if (aCompound._compoundID < 1 || aCompound._compoundName == "") {
		::MessageBox(NULL, _T("���׿��ڲ����ڸû�����"), _T("֪ͨ"), MB_OK);
		clearCompoundsOnEditCtrls();
		return;
	}

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
void LibSettingView::clearCompoundsOnEditCtrls() {
	const CString E = _T("");
	GetDlgItem(IDC_EDIT_COMPOUND_NAME)->SetWindowText(E);
	GetDlgItem(IDC_EDIT_FORMULA)->SetWindowText(E);
	GetDlgItem(IDC_EDIT_MASSWEIGHT)->SetWindowText(E);
	GetDlgItem(IDC_EDIT_CASNO)->SetWindowText(E);
	GetDlgItem(IDC_EDIT_PEAKDATA)->SetWindowText(E);
}
int  LibSettingView::checkCompound(Compound &aCompound) {
	const std::string E = "";
	if (aCompound._compoundName == E || 
		aCompound._formula == E || 
		aCompound._casNo == E || 
		aCompound._peakData == E) {
		return CHECK_FIND_NULL;
	}

	const int massWeight = aCompound._massWeight;
	if (massWeight < 0) {
		return CHECK_FIND_MINUS;
	}

	// ��֤ PeakData
	int max_X = 0;
	int peakCount = 0;
	const std::string strPeakData = aCompound._peakData;
	bool checkPass = checkPeakDataString(strPeakData, max_X, peakCount);
	if (!checkPass) {
		return CHECK_PEAK_FAIL;
	}
	aCompound._maxX = max_X;
	aCompound._peakCount = peakCount;

	return CHECK_OK;
}
bool LibSettingView::checkPeakDataString(const std::string strPeakData, int& maxX, int& peakCount) {
	// strPeakData ��ʽ���磺12 110;13 220;14 999;15 25;26 12;27 58;28 179;29 20;40 22;41 110;42 425;43 11;
	// 1. X �ǵ�����
	// 2. X �� Y �ķ�Χ[1,9999]

	int prev_X = 0;

	std::string::size_type i = 0;
	std::string::size_type j = strPeakData.find(';');

	while (j != std::string::npos) {
		std::string strXY = strPeakData.substr(i, j-i);
		std::string::size_type w = strXY.find(' ');
		std::string strX = strXY.substr(0, w);
		std::string strY = strXY.substr(w+1, strXY.length());

		int x = atoi(strX.c_str());
		int y = atoi(strY.c_str());
		peakCount++;

		if (x < 1 || x > 10000) { return false; }
		if (y < 1 || y > 10000) { return false; }
		if (x <= prev_X) { return false; }
		prev_X = x;

		if (x > maxX) {
			maxX = x;
		}

		i = ++j;
		j = strPeakData.find(';', j);
	}

	return true;
}

BEGIN_MESSAGE_MAP(LibSettingView, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE_DB, &LibSettingView::OnBnClickedChooseDB)
	ON_BN_CLICKED(IDC_BUTTON_QUERY_COMPOUND, &LibSettingView::OnBnClickedQueryCompound)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_DB, &LibSettingView::OnBnClickedCreateDB)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_COMPOUND, &LibSettingView::OnBnClickedSaveCompound)
	ON_BN_CLICKED(IDC_BUTTON_DEL_COMPOUND, &LibSettingView::OnBnClickedDelCompound)
END_MESSAGE_MAP()


// LibSettingView ��Ϣ�������
// - �׿�
void LibSettingView::OnBnClickedChooseDB() {
	CString strPath = _T("");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("�׿����ݿ� (*.db)|*.db|All Files (*.*)|*.*||"), NULL);

	if (fileDlg.DoModal() == IDOK) {

		strPath = fileDlg.GetPathName();
		GetDlgItem(IDC_EDIT_DB_PATH)->SetWindowText(strPath);
		_currentDBPath = strPath;
	} else { 

		GetDlgItem(IDC_EDIT_DB_PATH)->SetWindowText(_currentDBPath);
		return; 
	}
}
void LibSettingView::OnBnClickedCreateDB() {

	CString cstrPath = _T("");
	CFileDialog fileDlg(FALSE, _T("db"), NULL, OFN_HIDEREADONLY, _T("�׿����ݿ� (*.db)|*.db||"), NULL);

	if (fileDlg.DoModal() == IDOK) {
		cstrPath = fileDlg.GetPathName();
	} else { return; }

	// TODO: ����Ƿ�������ͻ
	const std::string strPath = CT2A(cstrPath);
	SqliteController sqliteController(strPath);
	
	// �����±�
	bool success = sqliteController.initTables();
	if (success) {
		CString dbName = fileDlg.GetFileName();
		CString msgText = _T("�½�������� ") + dbName + _T(" �ɹ�");
		::MessageBox(NULL, msgText, _T("֪ͨ"), MB_OK);

		GetDlgItem(IDC_EDIT_DB_PATH)->SetWindowText(cstrPath);
		_currentDBPath = cstrPath;

	} else {
		::MessageBox(NULL, _T("�½�������� ʧ��"), _T("����"), MB_OK | MB_ICONWARNING);

		GetDlgItem(IDC_EDIT_DB_PATH)->SetWindowText(_T(""));
		_currentDBPath = _T("");
	}
}
// - ������
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
	std::string sqlitePath = CT2A(_currentDBPath);
	SqliteController sqliteController(sqlitePath);
	Compound compound = sqliteController.getCompound(compoundID);

	// ��ʾ���
	setCompoundsOnEditCtrls(compound);
}
void LibSettingView::OnBnClickedSaveCompound() {

	// ��鵱ǰ�׿��Ƿ�����޸�
	if (_currentDBPath == _defaultDBPath) {
		::MessageBox(NULL, _T("��ǰ�׿�ΪϵͳĬ���׿⣬�����޸�"), _T("����"), MB_OK | MB_ICONWARNING);
		return;
	}

	// ��֤compound��Ч��
	Compound aCompound;
	getCompoundsOnEditCtrls(aCompound);

	int checkResult = checkCompound(aCompound);
	if (checkResult != CHECK_OK) {

		if (checkResult == CHECK_FIND_NULL) {
			::MessageBox(NULL, _T("��ǰ��ӻ��������Ϣ������©����һ��"), _T("����"), MB_OK | MB_ICONWARNING);
		}

		if (checkResult == CHECK_FIND_MINUS) {
			::MessageBox(NULL, _T("��������ΧӦ���� 1~9999 ֮��"), _T("����"), MB_OK | MB_ICONWARNING);
		}

		if (checkResult == CHECK_PEAK_FAIL) {
			::MessageBox(NULL, _T("�����ݸ�ʽ���� Ӧ���� X1 Y1;X2 Y2;X3 Y3 (X1 < X2 < X3)"), _T("����"), MB_OK | MB_ICONWARNING);
		}

		return;
	}

	const std::string strPath = CT2A(_currentDBPath);
	SqliteController sqliteController(strPath);

	// TODO: �����Ƿ����ظ���¼
	
	// ϵͳ����ID��
	const int newCompoundID = sqliteController.maxCompoundID() + 1;
	aCompound._compoundID = newCompoundID;
	sqliteController.storeCompound(aCompound);

	// ͬʱ�����������
	sqliteController.storeFiltePoint(aCompound);
}
void LibSettingView::OnBnClickedDelCompound() {
	// ��鵱ǰ�׿��Ƿ�����޸�
	if (_currentDBPath == _defaultDBPath) {
		::MessageBox(NULL, _T("��ǰ�׿�ΪϵͳĬ���׿⣬�����޸�"), _T("����"), MB_OK | MB_ICONWARNING);
		return;
	}

	// �õ� CompoundID
	CString cstrCompoundID;
	GetDlgItem(IDC_EDIT_COMPOUND_ID)->GetWindowText(cstrCompoundID);
	int compoundID = _ttoi(cstrCompoundID);

	// �ӵ�ǰ�׿�����
	std::string sqlitePath = CT2A(_currentDBPath);
	SqliteController sqliteController(sqlitePath);
	//Compound compound = sqliteController.getCompound(compoundID);
	//compoundID = compound._compoundID;

	//if (compoundID < 1) {
	//	::MessageBox(NULL, _T("���׿��ڲ����ڸû�����"), _T("֪ͨ"), MB_OK);
	//	clearCompoundsOnEditCtrls();
	//	return;
	//}

	// ɾ�� ������������
	sqliteController.deleteCompound(compoundID);
	clearCompoundsOnEditCtrls();
}
