
// Client.h : Client 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.
#include "ClientBase.h"
#include "NetworkTCP.h"

// CClientApp:
// 이 클래스의 구현에 대해서는 Client.cpp을 참조하십시오.
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

// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 구현입니다.
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
