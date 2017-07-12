
// ServerDlg.h : ��� ����
//

#pragma once

#include "Server.h"

#define WM_USER_MESSAGE			WM_USER + 1

// CServerDlg ��ȭ ����
class CServerDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CServerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_DIALOG };
#endif

	protected:
	virtual void			DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.
	afx_msg void			OnDestroy();


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CServer					m_pServer;

public:
	LRESULT					OnMessage( WPARAM wParam, LPARAM lParam );
	afx_msg void OnBnClickedCancel();
};

extern CServerDlg*			g_pDlg;
