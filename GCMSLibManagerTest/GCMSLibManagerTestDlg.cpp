
// GCMSLibManagerTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GCMSLibManagerTest.h"
#include "GCMSLibManagerTestDlg.h"
#include "afxdialogex.h"

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
	ON_BN_CLICKED(IDC_BUTTON1, &CGCMSLibManagerTestDlg::OnBnClickedLibSearchStrategy)
	ON_BN_CLICKED(IDC_BUTTON2, &CGCMSLibManagerTestDlg::OnBnClickedLibSetting)
	ON_BN_CLICKED(IDC_BUTTON3, &CGCMSLibManagerTestDlg::OnBnClickedLibParaSearch)
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


// - Test


#include "..\\GCMSLibManager\GCMSLibManager.h"
#include "..\\GCMSLibManager\Compound.h"

void CGCMSLibManagerTestDlg::OnBnClickedLibSearchStrategy()
{
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


	CString str(libConfig._dbPath1st.c_str());

	GCMSLibSearchStrategy(libConfig, this);
}


void CGCMSLibManagerTestDlg::OnBnClickedLibSetting()
{
	const CString libPathName = CString(_T("E:\\GCMSLibSearch\\GCMSLibSearch\\nist.db"));
	GCMSLibSetting(libPathName, this);
}


void CGCMSLibManagerTestDlg::OnBnClickedLibParaSearch()
{
	const CString libPathName = CString(_T("E:\\GCMSLibSearch\\GCMSLibSearch\\nist.db"));
	GCMSLibParaSearch(libPathName, this);
}
