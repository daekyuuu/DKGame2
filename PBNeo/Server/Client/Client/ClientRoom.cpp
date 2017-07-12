// ClientRoom.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Client.h"
#include "ClientRoom.h"


// CClientRoom

IMPLEMENT_DYNCREATE(CClientRoom, CClientBase)

CClientRoom::CClientRoom()
	: CClientBase(IDD_CLIENT_ROOM)
	, m_strTitle(_T(""))
	, m_strTimer(_T(""))
	, m_strState(_T(""))
{

}

CClientRoom::~CClientRoom()
{
}

void CClientRoom::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ROOM_TITLE, m_strTitle);
	DDX_Control(pDX, IDC_ROOM_SLOT_LIST, m_ctSlotList);
	DDX_Text(pDX, IDC_ROOM_TIMER, m_strTimer);
	DDX_Text(pDX, IDC_ROOM_STATE, m_strState);
}

BEGIN_MESSAGE_MAP(CClientRoom, CFormView)
	ON_BN_CLICKED(IDC_ROOM_LEAVE, &CClientRoom::OnBnClickedRoomLeave)
	ON_BN_CLICKED(IDC_ROOM_MAPVOTE, &CClientRoom::OnBnClickedRoomMapvote)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ROOM_READY, &CClientRoom::OnBnClickedRoomReady)
END_MESSAGE_MAP()


// CClientRoom 진단입니다.

#ifdef _DEBUG
void CClientRoom::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CClientRoom::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CClientRoom 메시지 처리기입니다.
void CClientRoom::OnInitialUpdate()
{
	CClientBase::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	LV_COLUMN lvColumn;
	m_ctSlotList.SetExtendedStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );	
	lvColumn.mask		= LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvColumn.fmt		= LVCFMT_CENTER;

	lvColumn.pszText	= _T("Nick");
	lvColumn.iSubItem	= 0;
	lvColumn.cx			= 150;
	m_ctSlotList.InsertColumn( 0, &lvColumn );

	lvColumn.pszText	= _T("Team");
	lvColumn.iSubItem	= 1;
	lvColumn.cx			= 50;
	m_ctSlotList.InsertColumn( 1, &lvColumn );	

	lvColumn.pszText	= _T("State");
	lvColumn.iSubItem	= 2;
	lvColumn.cx			= 50;
	m_ctSlotList.InsertColumn( 2, &lvColumn );

	for( INT32 i = 0 ; i < NET_ROOM_USER_COUNT ; i++ )
	{
		m_ctSlotList.InsertItem( i, _T("") );
	}

	SetTimer( 0, 1000, NULL );
}

void CClientRoom::_ClearSlot()
{
	for( INT32 i = 0 ; i < NET_ROOM_USER_COUNT ; i++ )
	{
		_ClearSlot( i );
	}
}

void CClientRoom::_ClearSlot( INT32 i32Idx )
{
	m_ctSlotList.SetItem( i32Idx, 0, LVIF_TEXT, _T(""), 0, 0, 0, 0 );
	m_ctSlotList.SetItem( i32Idx, 1, LVIF_TEXT, _T(""), 0, 0, 0, 0 );
	m_ctSlotList.SetItem( i32Idx, 2, LVIF_TEXT, _T(""), 0, 0, 0, 0 );
}

void CClientRoom::OnBnClickedRoomLeave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	PacketRoomLeaveReq Packet;
	m_pTCP->SendPacket( &Packet );
}

void CClientRoom::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch( m_eRoomState )
	{
	case NET_ROOM_STATE_READY:
	case NET_ROOM_STATE_COUNTDOWN:
		{
			UpdateData( TRUE );
			INT32 i32Time = _wtoi( m_strTimer.GetBuffer() ) - 1;
			m_strTimer.Format( _T("%02d"), i32Time );
			UpdateData( FALSE );
		}
		break;
	}

	CClientBase::OnTimer(nIDEvent);
}

void CClientRoom::SetRoomInfo( NET_ROOM_INFO* pRoomInfo )
{
	TCHAR strTitle[ NET_ROOM_TITLE_LENGTH ];
	pRoomInfo->m_strTitle.GetValue( strTitle, NET_ROOM_TITLE_LENGTH );
	m_strTitle = strTitle;

	switch( (NET_ROOM_MODE)(INT8)pRoomInfo->m_i8RoomMode )
	{
	case NET_ROOM_MODE_DEATHMATCH:
	case NET_ROOM_MODE_BOMBMISSION:
		m_i32TeamCount = 2;
		break;
	}

	UpdateData( FALSE );
}

void CClientRoom::EnterUser( PacketRoomEnterAck* pPacket )
{
	_ClearSlot();
	SetRoomInfo( &pPacket->m_RoomInfo );

	TCHAR strNick[ NET_NICK_LENGTH ];
	CString strTemp;
	NET_ROOM_SLOT_INFO* pSlot	= &pPacket->m_SlotInfo;
	INT32 i32SlotIdx			= _GetSlotIdx( pSlot->m_i8TeamIdx, pSlot->m_i8SlotIdx );
	
	pSlot->m_strNickname.GetValue( strNick, NET_NICK_LENGTH );
	m_ctSlotList.SetItem( i32SlotIdx, 0, LVIF_TEXT, strNick, 0, 0, 0, 0 );
	strTemp.Format( _T("%d"), (INT32)pSlot->m_i8TeamIdx );
	m_ctSlotList.SetItem( i32SlotIdx, 1, LVIF_TEXT, strTemp, 0, 0, 0, 0 );
	m_ctSlotList.SetItem( i32SlotIdx, 2, LVIF_TEXT, _GetStateSlotText( (NET_ROOM_SLOT_STATE)((INT8)pSlot->m_i8SlotState) ), 0, 0, 0, 0 );

	m_eRoomState	= (NET_ROOM_STATE)(INT32)pPacket->m_RoomInfo.m_ui8State;
	m_strState		= _GetStateText( m_eRoomState );
	m_strTimer.Format( _T("%02d"), (INT32)pPacket->m_ui8ReminTimeSec );
	UpdateData( FALSE );
}

void CClientRoom::LeaveUser()
{
}

void CClientRoom::EnterTransUser( PacketRoomEnterTransAck* pPacket )
{
	NET_ROOM_SLOT_INFO* pSlot	= &pPacket->m_SlotInfo;
	INT32 i32SlotIdx			= _GetSlotIdx( pSlot->m_i8TeamIdx, pSlot->m_i8SlotIdx );

	CString strTemp;
	TCHAR strNick[ NET_NICK_LENGTH ];
	pPacket->m_SlotInfo.m_strNickname.GetValue( strNick, NET_NICK_LENGTH );
	m_ctSlotList.SetItem( i32SlotIdx, 0, LVIF_TEXT, strNick, 0, 0, 0, 0 );
	strTemp.Format( _T("%d"), (INT32)pSlot->m_i8TeamIdx );
	m_ctSlotList.SetItem( i32SlotIdx, 1, LVIF_TEXT, strTemp, 0, 0, 0, 0 );
	m_ctSlotList.SetItem( i32SlotIdx, 2, LVIF_TEXT, _GetStateSlotText( (NET_ROOM_SLOT_STATE)((INT8)pSlot->m_i8SlotState) ), 0, 0, 0, 0 );
}

void CClientRoom::LeaveTransUser( PacketRoomLeaveTransAck* pPacket )
{
	_ClearSlot( _GetSlotIdx( pPacket->m_SlotBase.m_i8TeamIdx, pPacket->m_SlotBase.m_i8SlotIdx) );
}

void CClientRoom::RoomMapVote( PacketRoomMapVoteTransAck* pPacket )
{
	INT32 kk = 0;
}

void CClientRoom::RoomSlotStateChange( PacketRoomSlotStateChangeAck* pPacket )
{
	NET_ROOM_SLOT_BASE* pSlot;
	INT32 i32SlotIdx;
	for( INT32 i = 0 ; i < pPacket->m_SlotBase.GetCount() ; i++ )
	{
		pSlot = &pPacket->m_SlotBase[i];
		i32SlotIdx = _GetSlotIdx( pSlot->m_i8TeamIdx, pSlot->m_i8SlotIdx );

		m_ctSlotList.SetItem( i32SlotIdx, 2, LVIF_TEXT, _GetStateSlotText( (NET_ROOM_SLOT_STATE)((INT8)pSlot->m_i8SlotState) ), 0, 0, 0, 0 );
	}
}

void CClientRoom::RoomStateChange( PacketRoomStateChangeAck* pPacket )
{
	m_eRoomState	= (NET_ROOM_STATE)(INT32)pPacket->m_i8State;
	m_strState		= _GetStateText( m_eRoomState );
	m_strTimer.Format( _T("%02d"), (INT32)pPacket->m_ui8ReminTimeSec );
	UpdateData( FALSE );
}

void CClientRoom::SetRoomSlotInfo( PacketRoomSlotInfoAck* pPacket )
{
	TCHAR strNick[ NET_NICK_LENGTH ];
	CString strTemp;
	NET_ROOM_SLOT_INFO* pSlot;
	INT32 i32SlotIdx;
	
	for( INT32 i = 0 ; i < pPacket->m_SlotInfo.GetCount() ; i++ )
	{
		pSlot = &pPacket->m_SlotInfo[ i ];
		i32SlotIdx = _GetSlotIdx( pSlot->m_i8TeamIdx, pSlot->m_i8SlotIdx );
		pSlot->m_strNickname.GetValue( strNick, NET_NICK_LENGTH );
		m_ctSlotList.SetItem( i32SlotIdx, 0, LVIF_TEXT, strNick, 0, 0, 0, 0 );
		strTemp.Format( _T("%d"), (INT32)pSlot->m_i8TeamIdx );
		m_ctSlotList.SetItem( i32SlotIdx, 1, LVIF_TEXT, strTemp, 0, 0, 0, 0 );
		m_ctSlotList.SetItem( i32SlotIdx, 2, LVIF_TEXT, _GetStateSlotText( (NET_ROOM_SLOT_STATE)((INT8)pSlot->m_i8SlotState) ), 0, 0, 0, 0 );
	}
}

INT32 CClientRoom::_GetSlotIdx( INT8 i8TeamIdx, INT8 i8SlotIdx )
{
	return (i8SlotIdx * m_i32TeamCount) + i8TeamIdx;
}

TCHAR* CClientRoom::_GetStateText( NET_ROOM_STATE eState )
{
	switch( eState )
	{
	case NET_ROOM_STATE_CLOSE:		return _T("Close");
	case NET_ROOM_STATE_OPEN:		return _T("Open");
	case NET_ROOM_STATE_READY:		return _T("Ready");
	case NET_ROOM_STATE_COUNTDOWN:	return _T("CountDown");
	case NET_ROOM_STATE_PLAYING:	return _T("Playing");
	}
	return _T("None");
}

TCHAR* CClientRoom::_GetStateSlotText( NET_ROOM_SLOT_STATE eState )
{
	switch( eState )
	{
	case NET_ROOM_SLOT_STATE_CLOSE:		return _T("Close");
	case NET_ROOM_SLOT_STATE_OPEN:		return _T("Open");
	case NET_ROOM_SLOT_STATE_WAITING:	return _T("Waiting");
	case NET_ROOM_SLOT_STATE_READY:		return _T("Ready");
	case NET_ROOM_SLOT_STATE_INTRUSION:	return _T("Ready");
	case NET_ROOM_SLOT_STATE_PLAYING:	return _T("Playing");
	}
	return _T("None");
}

void CClientRoom::OnBnClickedRoomMapvote()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT32 i32MapIdx[ 16 ] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

	PacketRoomMapVoteReq Packet;
	Packet.m_ui32MapIdx = i32MapIdx[ rand()%16 ];
	m_pTCP->SendPacket( &Packet );
}


void CClientRoom::OnBnClickedRoomReady()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	PacketRoomSlotReadyReq Packet;
	m_pTCP->SendPacket( &Packet );
}
