#pragma once

#include "ClientBase.h"
#include "afxcmn.h"


// CClientRoom 폼 뷰입니다.

class CClientRoom : public CClientBase
{
	DECLARE_DYNCREATE(CClientRoom)

public:
	CClientRoom();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CClientRoom();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_ROOM };
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
	CString					m_strTitle;
	CListCtrl				m_ctSlotList;
	CString					m_strTimer;
	CString					m_strState;
	NET_ROOM_STATE			m_eRoomState;
	INT32					m_i32TeamCount;
		
	INT32					_GetSlotIdx( INT8 i8TeamIdx, INT8 i8SlotIdx );
	TCHAR*					_GetStateText( NET_ROOM_STATE eState );
	TCHAR*					_GetStateSlotText( NET_ROOM_SLOT_STATE eState );
	void					_ClearSlot();
	void					_ClearSlot( INT32 i32Idx );

public:	
	virtual void			OnInitialUpdate();
	afx_msg void			OnTimer(UINT_PTR nIDEvent);
	afx_msg void			OnBnClickedRoomLeave();
	afx_msg void			OnBnClickedRoomMapvote();	
	afx_msg void			OnBnClickedRoomReady();
	

	void					SetRoomInfo( NET_ROOM_INFO* pRoomInfo );
	void					SetRoomSlotInfo( PacketRoomSlotInfoAck* pPacket );

	void					EnterUser( PacketRoomEnterAck* pPacket );
	void					LeaveUser();
	void					EnterTransUser( PacketRoomEnterTransAck* pPacket );
	void					LeaveTransUser( PacketRoomLeaveTransAck* pPacket );

	void					RoomMapVote( PacketRoomMapVoteTransAck* pPacket );

	void					RoomStateChange( PacketRoomStateChangeAck* pPacket );
	void					RoomSlotStateChange( PacketRoomSlotStateChangeAck* pPacket );
	
	
};


