
// ServerDlg.h : 헤더 파일
//

#pragma once

#include "Server.h"

#define WM_USER_MESSAGE			WM_USER + 1

// CServerDlg 대화 상자
class CServerDlg : public CDialogEx
{
// 생성입니다.
public:
	CServerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_DIALOG };
#endif

	protected:
	virtual void			DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
	afx_msg void			OnDestroy();


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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
