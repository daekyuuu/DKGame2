#include "stdafx.h"
#include "S2Time.h"
#include <mmsystem.h>

#define SERVER_BEGIN_TIME	10000

static S2ThreadTimer*	s_pTimer;

static UINT32 _DefThreadProc( void * pPointer )
{
	((S2ThreadTimer*)pPointer)->OnUpdate(); 
	return 0; 
}

S2ThreadTimer::S2ThreadTimer()
{
}

S2ThreadTimer::~S2ThreadTimer()
{
	Destroy();
}

BOOL S2ThreadTimer::Create()
{
	if( FALSE == m_Thread.Create( (LPTHREAD_START_ROUTINE)_DefThreadProc, this ) )	return FALSE;

	return TRUE;
}

void S2ThreadTimer::Destroy()
{
	m_Thread.Destroy();
}

void S2ThreadTimer::OnUpdate()
{
	m_Thread.Init();

	SYSTEMTIME stSystemTime;

	while( m_Thread.IsRunning() )
	{
		// 썸머타임 영향받는 로컬 년월일시분초 업데이트		
		::GetLocalTime( &stSystemTime );
		m_dt32Local			= stSystemTime;

		// 썸머타임 관계없는 표준 년월일시분초 업데이트
		SYSTEMTIME UTCTime;
		TIME_ZONE_INFORMATION TZInfo;
		::GetSystemTime(&UTCTime);
		::GetTimeZoneInformation(&TZInfo);
		TZInfo.DaylightBias = 0;			// 타임존 정보에서 썸머타임을 적용하지 않도록 보정값 변경
		::SystemTimeToTzSpecificLocalTime( &TZInfo, &UTCTime, &stSystemTime );
		m_dt32Stand			= stSystemTime;

		// 4. 슬립
		m_Thread.Waiting( 1 );
	}
}

BOOL S2Time::Init(void)
{
	s_pTimer = new S2ThreadTimer;
	return s_pTimer->Create();
}

void S2Time::Destroy(void)
{
	SAFE_DELETE( s_pTimer );
}

DATE32 S2Time::LocalTime(void)
{
	return s_pTimer->GetLocalTime();
}

DATE32 S2Time::StandTime(void)
{
	return s_pTimer->GetStandTime();
}
