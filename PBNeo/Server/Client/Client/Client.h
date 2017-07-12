
// Client.h : Client ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.
#include "ClientBase.h"
#include "NetworkTCP.h"

// CClientApp:
// �� Ŭ������ ������ ���ؼ��� Client.cpp�� �����Ͻʽÿ�.
//

enum VIEW_PAGE
{
	VIEW_PAGE_CONNECT		= 0,
	VIEW_PAGE_LOGIN,
	VIEW_PAGE_LOBBY,
	VIEW_PAGE_ROOM,
	VIEW_PAGE_BATTLE,
	VIEW_PAGE_LOADOUT,

	VIEW_PAGE_COUNT,
};

class CClientApp : public CWinAppEx
{
public:
	CClientApp();
	virtual ~CClientApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

private:
	CClientBase*			m_pPage[ VIEW_PAGE_COUNT ];

	CNetworkTCP*			m_pTCP;
	
protected:
	CClientBase*			_GetPage( INT32 i32Number );
	BOOL					_CreatePage();

public:
	virtual BOOL			OnIdle(LONG lCount);
		
	CView*					SwitchView( VIEW_PAGE ePage );

	BOOL					ConnectServerReq( UINT32 ui32IP, UINT16 ui16Port );
	void					ConnectServerAck( char* pBuffer );
	void					LoginAck( char* pBuffer );
	void					UserInfoLoadAck( char* pBuffer );
	void					UserChangeNickAck( char* pBuffer );
	void					RoomCreateAck( char* pBuffer );
	void					RoomEnterAck( char* pBuffer );
	void					RoomLeaveAck( char* pBuffer );
	void					RoomEnterTransAck( char* pBuffer );
	void					RoomLeaveTransAck( char* pBuffer );

	void					RoomMapVoteAck( char* pBuffer );	

	void					RoomSlotInfoAck( char* pBuffer );	
	void					RoomSlotStateChangeAck( char* pBuffer );

	void					RoomStateChangeAck( char* pBuffer );
	void					RoomBattleStartAck( char* pBuffer );
	void					RoomBattleEndAck( char* pBuffer );
	void					RoomList( char* pBuffer );
	
};

extern CClientApp theApp;
