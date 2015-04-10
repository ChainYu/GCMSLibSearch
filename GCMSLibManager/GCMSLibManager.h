// GCMSLibManager.h : GCMSLibManager DLL ����ͷ�ļ�
#pragma once
#include "StdAfx.h" 
#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "Resource.h"		
#include "Compound.h"
#include <vector>


#define LIBMANAGERDLL_EXPORTS
#ifdef  LIBMANAGERDLL_EXPORTS
#define LIBMANAGERDLL_API __declspec(dllexport)
#else
#define LIBMANAGERDLL_API __declspec(dllimport) 
#endif

// - ���� DLL - //

// ���׿�����㷨��
// ���룺�����������ò��� libConfig
//		 ������Ʒ������
// ������׿⻯������
LIBMANAGERDLL_API VOID GCMSLibSearch(const LibConfig& libConfig, Compound testCompound, std::vector<Compound> &libCompounds);

// ���׿�������� �Ի�����桿
// ���룺���ò��� libConfig
// ������޸ĺ�����ò��� Ref
// ���أ������Ƿ�ı�
LIBMANAGERDLL_API bool GCMSLibSearchStrategy(LibConfig& libConfig, CWnd* pParent = NULL);

// ���׿����ɾ�� �Ի�����桿
// ���룺 1.  Ĭ�����ݿ�·�� defaultDB
//		  2. ��ǰMS������ std::string peakData
LIBMANAGERDLL_API VOID GCMSLibSetting(const CString defaultLibPathName, CWnd* pParent = NULL);

// ���������� �Ի�����桿
// ���룺 1.Ĭ�����ݿ�·�� defaultDB
LIBMANAGERDLL_API VOID GCMSLibParaSearch(const CString defaultLibPathName, CWnd* pParent = NULL);



// ���ñ���ǰ��ɾ
class CGCMSLibManagerApp : public CWinApp {
public:
	CGCMSLibManagerApp() { }

// ��д
public:
	virtual BOOL InitInstance() { CWinApp::InitInstance(); return TRUE; }

	DECLARE_MESSAGE_MAP()
};


