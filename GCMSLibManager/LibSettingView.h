#pragma once

#include "GCMSLibManager.h"
// LibSettingView �Ի���

class LibSettingView : public CDialogEx
{
	DECLARE_DYNAMIC(LibSettingView)

public:
	LibSettingView(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~LibSettingView();

// �Ի�������
	enum { IDD = IDD_LIBSETTINGVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
