
// GCMSLibSearch.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CGCMSLibSearchApp:
// �йش����ʵ�֣������ GCMSLibSearch.cpp
//

class CGCMSLibSearchApp : public CWinApp
{
public:
	CGCMSLibSearchApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CGCMSLibSearchApp theApp;