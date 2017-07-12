#pragma once

#include "ClientBase.h"
#include "afxcmn.h"

// CClientLobby 폼 뷰입니다.

class CClientLobby : public CClientBase
{
	DECLARE_DYNCREATE(CClientLobby)

public:
	CClientLobby();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CClientLobby();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_LOBBY };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
protected:
	CString					m_strNick;
	int						m_i32Level;
	CListCtrl				m_ctLobbyRoomList;

	TCHAR*					_GetStateRoomText( NET_ROOM_STATE eState );

public:

	void					SetUserInfo( PacketUserInfoLoadAck* pPacket );
	void					SetUserNick( PacketUserChangeNickAck* pPacket );
	void					RoomList( PacketRoomListAck* pPacket );

	virtual void			OnInitialUpdate();
	afx_msg void			OnBnClickedLobbyUserdata();
	afx_msg void			OnBnClickedLobbyCreateRoom();
	afx_msg void			OnBnClickedLobbyEnterRoom();
	
	afx_msg void			OnBnClickedLobbyGetRoomList();
	
	
};


