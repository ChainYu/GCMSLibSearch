
// GCMSLibManagerTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GCMSLibManagerTest.h"
#include "GCMSLibManagerTestDlg.h"
#include "afxdialogex.h"

#include "..\\GCMSLibManager\GCMSLibManager.h"
#include "..\\GCMSLibManager\Compound.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGCMSLibManagerTestDlg �Ի���




CGCMSLibManagerTestDlg::CGCMSLibManagerTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGCMSLibManagerTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGCMSLibManagerTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGCMSLibManagerTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LIB_SEARCH, &CGCMSLibManagerTestDlg::OnBnClickedButtonLibSearch)
	ON_BN_CLICKED(IDC_BUTTON_LIB_STRATEGY, &CGCMSLibManagerTestDlg::OnBnClickedButtonLibStrategy)
	ON_BN_CLICKED(IDC_BUTTON_LIB_SETTING, &CGCMSLibManagerTestDlg::OnBnClickedButtonLibSetting)
	ON_BN_CLICKED(IDC_BUTTON_PARA_SEARCH, &CGCMSLibManagerTestDlg::OnBnClickedButtonParaSearch)
END_MESSAGE_MAP()


// CGCMSLibManagerTestDlg ��Ϣ�������

BOOL CGCMSLibManagerTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CGCMSLibManagerTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CGCMSLibManagerTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CGCMSLibManagerTestDlg::OnBnClickedButtonLibSearch() {
	LibConfig libConfig;
	libConfig._dbPath1st = "E:\\GCMSLibSearch\\db\\nist.db";
	GCMSLibSearchStrategy(libConfig, this);

	Compound testCompound;
	testCompound._compoundID = 100;
	testCompound._compoundName = "2-Bromoethyl isothiocyanate";
	testCompound._formula = "C3H4BrNS";
	testCompound._massWeight = 165;
	testCompound._casNo = "1483-41-6";
	testCompound._peakCount = 66;
	testCompound._peakData = "12 11;13 11;15 24;24 6;25 36;26 267;27 999;28 98;29 8;32 9;\
							 33 11;34 10;37 5;38 28;39 31;40 59;41 13;42 16;43 5;44 102;45 120;46 24;47 6;50 6;51 39;\
							 52 39;53 5;54 7;56 15;57 56;58 224;59 242;60 287;61 20;62 13;70 47;72 541;73 21;74 22;79 70;\
							 80 22;81 67;82 28;83 13;84 50;85 49;86 76;87 5;88 4;91 9;92 7;93 49;94 9;95 32;104 4;\
							 105 13;107 648;109 621;110 16;113 4;165 509;166 21;167 486;168 21;169 22;170 0;";

	std::vector<Compound> libCompounds;
	GCMSLibSearch(libConfig, testCompound, libCompounds);
}


void CGCMSLibManagerTestDlg::OnBnClickedButtonLibStrategy() {
	LibConfig libConfig;
	libConfig._dbPath1st = "E:\\GCMSLibSearch\\GCMSLibSearch\\nist.db";
	libConfig._dbPath2nd = "fakfklajsfklaj";
	libConfig._dbPath3rd = "dfa3";
	libConfig._minMass1st = 10;
	libConfig._minMass2nd = 20;
	libConfig._minMass3rd = 30;
	libConfig._lowerMass = 1;
	libConfig._upperMass = 999;
	libConfig._matchLimitNumber = 20;
	libConfig._isUnique = true;

	GCMSLibSearchStrategy(libConfig, this);
}


void CGCMSLibManagerTestDlg::OnBnClickedButtonLibSetting() {
	const CString libPathName = CString(_T("E:\\GCMSLibSearch\\GCMSLibSearch\\nist.db"));
	GCMSLibSetting(libPathName, this);
}


void CGCMSLibManagerTestDlg::OnBnClickedButtonParaSearch() {
	const CString libPathName = CString(_T("E:\\GCMSLibSearch\\GCMSLibSearch\\nist.db"));
	GCMSLibParaSearch(libPathName, this);
}
