#pragma once

#include "GCMSLibManager.h"

#define CHECK_OK			1
#define CHECK_FIND_NULL		2
#define CHECK_FIND_MINUS	3
#define CHECK_PEAK_FAIL		4
#define CHECK_MASS_FAIL		5



// LibSettingView �Ի���
class LibSettingView : public CDialogEx
{
	DECLARE_DYNAMIC(LibSettingView)

public:
	LibSettingView(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~LibSettingView();

// �Ի�������
	enum { IDD = IDD_LIBSETTINGVIEW };

	afx_msg void OnBnClickedChooseDB();
	afx_msg void OnBnClickedCreateDB();
	afx_msg void OnBnClickedQueryCompound();
	afx_msg void OnBnClickedSaveCompound();
	afx_msg void OnBnClickedDelCompound();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	void setCompoundsOnEditCtrls(const Compound &aCompound);
	void getCompoundsOnEditCtrls(Compound &aCompound);
	void clearCompoundsOnEditCtrls();
	int  checkCompound(Compound &aCompound); // �ڼ���Ҫ��maxXд��compound3
	bool checkPeakDataString(const std::string peakData, int& maxX, int& peakCount);

public:
	CString _defaultDBPath; // ϵͳĬ���׿�
	CString _currentDBPath; // ��ǰʹ���׿�
	
};
