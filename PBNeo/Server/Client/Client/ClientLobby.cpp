// ClientLobby.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Client.h"
#include "ClientLobby.h"

#include "ChangeNick.h"

// CClientLobby

IMPLEMENT_DYNCREATE(CClientLobby, CClientBase)

CClientLobby::CClientLobby()
	: CClientBase(IDD_CLIENT_LOBBY)
	, m_strNick(_T(""))
	, m_i32Level(0)
{

}

CClientLobby::~CClientLobby()
{
}

void CClientLobby::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LOBBY_NICK, m_strNick);
	DDX_Text(pDX, IDC_LOBBY_LEVEL, m_i32Level);
	DDX_Control(pDX, IDC_LOBBY_ROOMLIST, m_ctLobbyRoomList);
}

BEGIN_MESSAGE_MAP(CClientLobby, CFormView)
	ON_BN_CLICKED(IDC_LOBBY_USERDATA, &CClientLobby::OnBnClickedLobbyUserdata)
	ON_BN_CLICKED(IDC_LOBBY_CREATE_ROOM, &CClientLobby::OnBnClickedLobbyCreateRoom)
	ON_BN_CLICKED(IDC_LOBBY_ENTER_ROOM, &CClientLobby::OnBnClickedLobbyEnterRoom)
	ON_BN_CLICKED(IDC_LOBBY_GET_ROOM_LIST, &CClientLobby::OnBnClickedLobbyGetRoomList)
END_MESSAGE_MAP()


// CClientLobby 진단입니다.

#ifdef _DEBUG
void CClientLobby::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CClientLobby::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CClientLobby 메시지 처리기입니다.
void CClientLobby::OnInitialUpdate()
{
	CClientBase::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	LV_COLUMN lvColumn;
	m_ctLobbyRoomList.SetExtendedStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );	
	lvColumn.mask		= LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvColumn.fmt		= LVCFMT_CENTER;

	lvColumn.pszText	= _T("Idx");
	lvColumn.iSubItem	= 0;
	lvColumn.cx			= 50;
	m_ctLobbyRoomList.InsertColumn( 0, &lvColumn );

	lvColumn.pszText	= _T("Title");
	lvColumn.iSubItem	= 1;
	lvColumn.cx			= 150;
	m_ctLobbyRoomList.InsertColumn( 1, &lvColumn );

	lvColumn.pszText	= _T("User");
	lvColumn.iSubItem	= 2;
	lvColumn.cx			= 50;
	m_ctLobbyRoomList.InsertColumn( 2, &lvColumn );

	lvColumn.pszText	= _T("State");
	lvColumn.iSubItem	= 3;
	lvColumn.cx			= 50;
	m_ctLobbyRoomList.InsertColumn( 3, &lvColumn );
}

void CClientLobby::SetUserInfo( PacketUserInfoLoadAck* pPacket )
{
	switch( pPacket->m_TResult )
	{
	case NET_RESULT_SUCCESS_FIRST_CONNECT:
		{
			CChangeNick dlg;
			dlg.DoModal();
			PacketUserChangeNickReq Packet;
			Packet.m_strNickname = dlg.m_strNickname.GetBuffer();
			m_pTCP->SendPacket( &Packet );
			// 이름 선택
		}
		break;
	default:
		{
			TCHAR strNick[ NET_NICK_LENGTH ];
			pPacket->m_Base.m_strNickname.GetValue( strNick, NET_NICK_LENGTH );
			m_strNick	= strNick;
			m_i32Level	= pPacket->m_Base.m_ui16Level;
			UpdateData( FALSE );
		}
		break;
	}	
}

void CClientLobby::SetUserNick( PacketUserChangeNickAck* pPacket )
{
	if( S2MO_IS_SUCCESS( pPacket->m_TResult ) )
	{
		TCHAR strNick[ NET_NICK_LENGTH ];
		pPacket->m_strNickname.GetValue( strNick, NET_NICK_LENGTH );
		
		m_strNick = strNick;
		UpdateData( FALSE );
	}
}

void CClientLobby::RoomList( PacketRoomListAck* pPacket )
{
	m_ctLobbyRoomList.DeleteAllItems();

	CString strTemp;
	NET_ROOM_INFO* pRoom;
	TCHAR strTitle[ NET_ROOM_TITLE_LENGTH ];
	for( INT32 i = 0 ; i < pPacket->m_RoomList.GetCount() ; i++ )
	{
		pRoom = &pPacket->m_RoomList[i];
		strTemp.Format( _T("%d"), (INT32)pRoom->m_ui32RoomIdx );
		m_ctLobbyRoomList.InsertItem( i, strTemp );

		pRoom->m_strTitle.GetValue( strTitle, NET_ROOM_TITLE_LENGTH );
		m_ctLobbyRoomList.SetItem( i, 1, LVIF_TEXT, strTitle, 0, 0, 0, 0 );

		strTemp.Format( _T("%d/%d"), (INT32)pRoom->m_ui8UserCountCur, (INT32)pRoom->m_ui8UserCountMax );
		m_ctLobbyRoomList.SetItem( i, 2, LVIF_TEXT, strTemp, 0, 0, 0, 0 );

		m_ctLobbyRoomList.SetItem( i, 3, LVIF_TEXT, _GetStateRoomText( (NET_ROOM_STATE)(UINT8)pRoom->m_ui8State ), 0, 0, 0, 0 );
	}
}

void CClientLobby::OnBnClickedLobbyUserdata()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	PacketUserInfoLoadReq Packet;	
	m_pTCP->SendPacket( &Packet );
}

void CClientLobby::OnBnClickedLobbyCreateRoom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	PacketRoomCreateReq Packet;
	Packet.m_RoomCreateInfo.m_strTitle				= _T("Room1");
	Packet.m_RoomCreateInfo.m_i8RoomMode			= NET_ROOM_MODE_DEATHMATCH;
	Packet.m_RoomCreateInfo.m_ui16TimeToRoundSec	= 300;
	Packet.m_RoomCreateInfo.m_ui8MemberCountOfTeam	= 8;
	m_pTCP->SendPacket( &Packet );
}

void CClientLobby::OnBnClickedLobbyEnterRoom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	PacketRoomEnterReq Packet;
	Packet.m_ui32RoomIdx						= (UINT32)0;
	m_pTCP->SendPacket( &Packet );
}


void CClientLobby::OnBnClickedLobbyGetRoomList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	PacketRoomListReq Packet;
	m_pTCP->SendPacket( &Packet );
}

TCHAR* CClientLobby::_GetStateRoomText( NET_ROOM_STATE eState )
{
	switch( eState )
	{
	case NET_ROOM_STATE_CLOSE:		return _T("Close");
	case NET_ROOM_STATE_OPEN:
	case NET_ROOM_STATE_READY:
	case NET_ROOM_STATE_COUNTDOWN:	return _T("Waiting");
	case NET_ROOM_STATE_PLAYING:	return _T("Playing");
	}
	return _T("None");
}