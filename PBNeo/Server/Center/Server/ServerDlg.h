
// ServerDlg.h : ��� ����
//

#pragma once

#include "Server.h"
#include "afxcmn.h"

#define WM_USER_MESSAGE			WM_USER + 1


#include "UserContainer.h"
#include "Room.h"

enum DLG_MESSAGE
{
	DLG_MESSAGE_BASE,

	DLG_MESSAGE_SERVER_INSERT,
	DLG_MESSAGE_SERVER_DESTROY,

	DLG_MESSAGE_USER_INSERT,
	DLG_MESSAGE_USER_DELETE,
	DLG_MESSAGE_USER_UPDATE,

	DLG_MESSAGE_ROOM_CREATE,
	DLG_MESSAGE_ROOM_DESTROY,
	DLG_MESSAGE_ROOM_UPDATE,
};

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

	void					_InsertServer( SERVER_TYPE_INFO* pInfo );
	void					_DestroyServer( SERVER_TYPE_INFO* pInfo );

	TCHAR*					_GetUserState( USER_STATE eState );

	void					_UserInsert( USER_LIST* pUser );
	void					_UserDelete( USER_LIST* pUser );
	void					_UserUpdate( USER_LIST* pUser );

	TCHAR*					_GetRoomState( NET_ROOM_STATE eState );

	void					_RoomCreate( CRoom* pRoom );
	void					_RoomDestroy( CRoom* pRoom );
	void					_RoomUpdate( CRoom* pRoom );

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
	CListCtrl				m_ctUserList;
	CListCtrl				m_ctRoomList;
	CTreeCtrl				m_ctServerList;

public:
	LRESULT					OnMessage( WPARAM wParam, LPARAM lParam );
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtNotice();
	CString m_strNotice;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedCancel();
};

extern CServerDlg*			g_pDlg;