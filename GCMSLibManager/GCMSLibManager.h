// GCMSLibManager.h : GCMSLibManager DLL ����ͷ�ļ�
//

#pragma once
#include "StdAfx.h" 

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "Resource.h"		
#include "Compound.h"
//#include "../GCMSLibSearch/Compound.h"


// CGCMSLibManagerApp
// �йش���ʵ�ֵ���Ϣ������� GCMSLibManager.cpp
//

#define LIBMANAGERDLL_EXPORTS
#ifdef LIBMANAGERDLL_EXPORTS
#define LIBMANAGERDLL_API __declspec(dllexport)
#else
#define LIBMANAGERDLL_API __declspec(dllimport) 
#endif

// - ���� DLL - //

// ���׿�������� �Ի�����桿
// ���룺���ò��� libConfig
// ������޸ĺ�����ò��� Ref
LIBMANAGERDLL_API VOID GCMSLibSearchStrategy(CWnd* pParent, LibConfig& libConfig);

// ���׿����ɾ�� �Ի�����桿
// ���룺 1.Ĭ�����ݿ�·�� defaultDB
//		  2. ��ǰMS������ std::string peakData
LIBMANAGERDLL_API VOID GCMSLibSetting(CWnd* pParent);


class CGCMSLibManagerApp : public CWinApp {
public:
	CGCMSLibManagerApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
