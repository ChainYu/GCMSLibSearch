// GCMSLibManager.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "GCMSLibManager.h"
#include "LibSearchStrategyView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define  LIBMANAGERDLL_API extern "C" __declspec(dllexport)

BOOL GCMSLibSearchStrategy(CWnd* pParent, LibConfig& libConfig) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	LibSearchStrategyView libManagerView(pParent, libConfig);
	if (libManagerView.DoModal()) {
		libConfig = libManagerView._libConfig;
	}
	
	return TRUE;
}


//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction(CWnd* parent)
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());			
//			// �˴�Ϊ��ͨ������
//			CXXXDlg dlg(parent);
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CGCMSLibManagerApp

BEGIN_MESSAGE_MAP(CGCMSLibManagerApp, CWinApp)
END_MESSAGE_MAP()


// CGCMSLibManagerApp ����

CGCMSLibManagerApp::CGCMSLibManagerApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CGCMSLibManagerApp ����

CGCMSLibManagerApp theApp;


// CGCMSLibManagerApp ��ʼ��

BOOL CGCMSLibManagerApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
