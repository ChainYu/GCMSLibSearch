
// GCMSLibManagerTestDlg.h : ͷ�ļ�
//

#pragma once


// CGCMSLibManagerTestDlg �Ի���
class CGCMSLibManagerTestDlg : public CDialogEx
{
// ����
public:
	CGCMSLibManagerTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_GCMSLIBMANAGERTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButtonLibSearch();
	afx_msg void OnBnClickedButtonLibStrategy();
	afx_msg void OnBnClickedButtonLibSetting();
	afx_msg void OnBnClickedButtonParaSearch();

	DECLARE_MESSAGE_MAP()
};
