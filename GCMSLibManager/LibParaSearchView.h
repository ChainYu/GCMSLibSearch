#pragma once

#include "GCMSLibManager.h"
// LibParaSearchView �Ի���

class LibParaSearchView : public CDialogEx
{
	DECLARE_DYNAMIC(LibParaSearchView)

public:
	LibParaSearchView(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~LibParaSearchView();

// �Ի�������
	enum { IDD = IDD_LIBPARASEARCHVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
