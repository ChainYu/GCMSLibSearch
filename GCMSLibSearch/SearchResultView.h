#pragma once


// CSearchResultView �Ի���

class CSearchResultView : public CDialog
{
	DECLARE_DYNAMIC(CSearchResultView)


// �Ի�������
	enum { IDD = IDD_SEARCH_RESULT_VIEW };

protected:
	CSearchResultView(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSearchResultView();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	
	DECLARE_MESSAGE_MAP()

//����ģʽ
protected:
	static CSearchResultView* _pInstance;

public:
	static CSearchResultView* getInstance();
	
	afx_msg void OnBnClickedCancel();
};



