// GCMSLibManager.h : GCMSLibManager DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "Resource.h"		 
#include "../GCMSLibSearch/Compound.h"


// CGCMSLibManagerApp
// �йش���ʵ�ֵ���Ϣ������� GCMSLibManager.cpp
//

#define LIBMANAGERDLL_EXPORTS
#ifdef LIBMANAGERDLL_EXPORTS
#define LIBMANAGERDLL_API __declspec(dllexport)
#else
#define LIBMANAGERDLL_API __declspec(dllimport) 
#endif

// ���� DLL
LIBMANAGERDLL_API BOOL GCMSLibSearchStrategy(CWnd* pParent = NULL);



class CGCMSLibManagerApp : public CWinApp
{
public:
	CGCMSLibManagerApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
