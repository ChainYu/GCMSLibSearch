// LibSettingView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LibSettingView.h"
#include "afxdialogex.h"


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


BEGIN_MESSAGE_MAP(LibSettingView, CDialogEx)
END_MESSAGE_MAP()


// LibSettingView ��Ϣ�������
