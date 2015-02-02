
// GCMSLibSearchDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "GCMSLibSearch.h"
#include "GCMSLibSearchDlg.h"


#include "SqliteController.h"



#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGCMSLibSearchDlg �Ի���




CGCMSLibSearchDlg::CGCMSLibSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGCMSLibSearchDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGCMSLibSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGCMSLibSearchDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CGCMSLibSearchDlg::OnBnLoadTestingPeakData)
	//ON_BN_CLICKED(IDCANCEL, &CGCMSLibSearchDlg::OnOpenSearchResult)
	ON_BN_CLICKED(IDC_BUTTON1, &CGCMSLibSearchDlg::OnBnShowSearchResult)
END_MESSAGE_MAP()


// CGCMSLibSearchDlg ��Ϣ�������

BOOL CGCMSLibSearchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	
	init();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CGCMSLibSearchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CGCMSLibSearchDlg::OnPaint()
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

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CGCMSLibSearchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGCMSLibSearchDlg::OnCancel() {
	CDialogEx::OnCancel();
}

void CGCMSLibSearchDlg::init() {
	CRect rcPeak, rcCompare;
	GetDlgItem(IDC_PEAK)->GetWindowRect(rcPeak);
	ScreenToClient(rcPeak);
	GetDlgItem(IDC_PEAK_COMPARE)->GetWindowRect(rcCompare);
	ScreenToClient(rcCompare);

	_peakChart.Create(this, rcPeak, 0, WS_CHILD|WS_VISIBLE);
	_compareChart.Create(this, rcCompare, 0, WS_CHILD|WS_VISIBLE);

	_peakDrawView.setChartCtrl(&_peakChart, &_compareChart);
}

void CGCMSLibSearchDlg::OnBnLoadTestingPeakData() {

	CString peakData(_T("12 108;13 229;14 999;15 22;26 18;27 58;28 178;29 23;40 18;41 108;42 431;43 8;"));

	_peakDrawView.drawPeak(peakData);

}

void CGCMSLibSearchDlg::drawPeakCompare(const CString &strPeakData) {
	_peakDrawView.drawPeakCompare(strPeakData);
}


void CGCMSLibSearchDlg::OnBnShowSearchResult()
{
	std::vector<Compound> compounds;

	//��ʼ�׿������߼�
	SqliteController sqliteController;
	sqliteController.queryCompoundData(compounds);

	//

	CSearchResultView *pSearchResultDlg = CSearchResultView::getInstance();
	pSearchResultDlg->fillCompoundList(compounds);
	pSearchResultDlg->setNofityObject(this);
	pSearchResultDlg->ShowWindow(SW_SHOW);

}


