#include "stdafx.h"
#include "Database.h"
#include "Config.h"

static UINT32 _DefThreadProc( void * pPointer )
{
	((CDatabase*)pPointer)->OnUpdate(); 
	return 0;
}

CDatabase::CDatabase()
{
	m_pDB			= NULL;

	m_pRingPush		= NULL;
	m_pRingPop		= NULL;
}

CDatabase::~CDatabase()
{
	Destroy();
}

BOOL CDatabase::Create( S2DB_CONNECT_INFO* pConnectInfo, S2RING_INFO* pPush, S2RING_INFO* pPop )
{
	m_pRingPush = new S2RingBuffer;
	if( NULL == m_pRingPush )												return FALSE;
	if( FALSE == m_pRingPush->Create( pPush ) )								return FALSE;

	m_pRingPop = new S2RingBuffer;
	if( NULL == m_pRingPop )												return FALSE;
	if( FALSE == m_pRingPop->Create( pPop ) )								return FALSE;

	m_pDB = new S2DBAdo;
	if( NULL == m_pDB )														return FALSE;
	if( FALSE == m_pDB->Create() )											return FALSE;

	if( FALSE == m_pDB->Connect( pConnectInfo ) )							return FALSE;

	if( m_Thread.Create( (LPTHREAD_START_ROUTINE)_DefThreadProc ,this ) )	return FALSE;
	return TRUE;
}

void CDatabase::Destroy()
{
	m_Thread.Destroy();

	SAFE_DELETE( m_pRingPush );
	SAFE_DELETE( m_pRingPop );
	SAFE_DELETE( m_pDB );
}

T_RESULT CDatabase::PushData( RING_DB_PUSH* pPush )
{
	if( RING_BUFFER_SUCCESS != m_pRingPush->Push( pPush ) )
	{
		return NET_RESULT_ERROR;
	}

	return NET_RESULT_SUCCESS;
}

void CDatabase::OnUpdate()
{
	m_Thread.Init();

	BOOL bWorking;
	while( m_Thread.IsRunning() )
	{
		bWorking = FALSE;

		if( 0 < m_pRingPush->GetBufferCount() )
		{
			bWorking = TRUE;

			RING_DB_PUSH* pPush = (RING_DB_PUSH*)m_pRingPush->Pop();
			RING_DB_POP* pPop	= (RING_DB_POP*)m_pRingPop->PushPointer();

			if( (NULL == pPush) || (NULL == pPop) )
			{
				m_Thread.Waiting( 500 );
				continue;
			}

			pPop->m_DBType			= pPush->m_DBType;
			pPop->m_i32SessionIdx	= pPush->m_i32SessionIdx;
			pPop->m_TUID			= pPush->m_TUID;

			switch( pPush->m_DBType )
			{
				case DB_TYPE_USERINFO_LOAD:
				{
					_GetUserInfo( pPush, pPop );
				}
				break;
				case DB_TYPE_USERINFO_SAVE:
				{
					_SetUserInfo( pPush, pPop );
				}
				break;
			}

			m_pRingPush->PopIdx();
			m_pRingPop->PushPointerIdx();
		}
		
		if( FALSE == bWorking )
		{
			m_Thread.Waiting( 1 );
		}
	}
}

void CDatabase::_GetUserInfo( RING_DB_PUSH* pPush, RING_DB_POP* pPop )
{
	TCHAR strSQL[ S2_STRING_COUNT ];

	pPop->m_TResult					= NET_RESULT_SUCCESS;

	INT32 i32Result;

	S2DB::MakeQuery( strSQL, S2_STRING_COUNT, _T("usp_UserInfo_sel"),
		S2DB_DATA_TYPE_BIGINT,					pPush->m_TUID,
		S2DB_DATA_TYPE_END );
	if( m_pDB->ExecuteSP( strSQL ) && m_pDB->GetRecordCount() )
	{
		i32Result = m_pDB->GetINT32Data( 2 );

		switch( i32Result )
		{
		case DB_RESULT_SUCCESS:				// 정상 처리
			{
				m_pDB->GetTextData( 4, pPop->m_Base.m_strNickname, NET_NICK_LENGTH );
				pPop->m_Base.m_ui16Level	= m_pDB->GetUINT16Data( 5 );
				pPop->m_Info.m_ui32Exp		= m_pDB->GetUINT32Data( 6 );
			}
			break;
		case DB_RESULT_EMPTYROW:			// 첫 접속 유저입니다.
			{	
				pPop->m_TResult = NET_RESULT_SUCCESS_FIRST_CONNECT;
				pPop->m_Base.Reset();
			}
			break;
		default:							// 알 수 없는 DB 에러
			{
				pPop->m_TResult				= NET_RESULT_ERROR_DB;
				return;
			}
			break;
		}
	}
	else
	{
		pPop->m_TResult						= NET_RESULT_ERROR_DB;
		return;
	}

	S2DB::MakeQuery( strSQL, S2_STRING_COUNT, _T("usp_UserRecordTotal_sel"),
		S2DB_DATA_TYPE_BIGINT,					pPush->m_TUID,
		S2DB_DATA_TYPE_END );
	if( m_pDB->ExecuteSP( strSQL ) && m_pDB->GetRecordCount())
	{
		i32Result = m_pDB->GetINT32Data( 2 );

		switch( i32Result )
		{
		case DB_RESULT_SUCCESS:				// 정상 처리
			{
				pPop->m_RecordTotal.m_ui32Match			= m_pDB->GetUINT32Data( 4 );
				pPop->m_RecordTotal.m_ui32Win			= m_pDB->GetUINT32Data( 5 );
				pPop->m_RecordTotal.m_ui32Lose			= m_pDB->GetUINT32Data( 6 );

				pPop->m_RecordTotal.m_ui32Kill			= m_pDB->GetUINT32Data( 7 );
				pPop->m_RecordTotal.m_ui32Death			= m_pDB->GetUINT32Data( 8 );
				pPop->m_RecordTotal.m_ui32HeadShot		= m_pDB->GetUINT32Data( 9 );
				pPop->m_RecordTotal.m_ui32Assist		= m_pDB->GetUINT32Data( 10 );

				pPop->m_RecordTotal.m_ui32PlayTime		= m_pDB->GetUINT32Data( 12 );
				pPop->m_RecordTotal.m_ui32MVPCount		= m_pDB->GetUINT32Data( 13 );
				pPop->m_RecordTotal.m_ui32HelmetProtect	= m_pDB->GetUINT32Data( 14 );
				pPop->m_RecordTotal.m_ui32BestScore		= m_pDB->GetUINT32Data( 15 );
				pPop->m_RecordTotal.m_ui32BestDamage	= m_pDB->GetUINT32Data( 16 );

				pPop->m_RecordTotal.m_ui32RankPoint		= m_pDB->GetUINT32Data( 17 );
			}
			break;
		case DB_RESULT_EMPTYROW:			// 첫 접속 유저입니다.
			{	
				pPop->m_RecordTotal.Reset();
			}
			break;
		default:							// 알 수 없는 DB 에러
			{
				pPop->m_TResult				= NET_RESULT_ERROR_DB;
				return;
			}
			break;
		}
	}
	else
	{
		pPop->m_TResult				= NET_RESULT_ERROR;
		return;
	}

	S2DB::MakeQuery( strSQL, S2_STRING_COUNT, _T("usp_UserRecordSeason_sel"),
		S2DB_DATA_TYPE_BIGINT,					pPush->m_TUID,
		S2DB_DATA_TYPE_INT,						g_pConfig->m_ui32Season,
		S2DB_DATA_TYPE_END );
	if( m_pDB->ExecuteSP( strSQL ) && m_pDB->GetRecordCount())
	{
		i32Result = m_pDB->GetINT32Data( 2 );

		switch( i32Result )
		{
		case DB_RESULT_SUCCESS:				// 정상 처리
			{
				pPop->m_RecordSeason.m_ui32Match			= m_pDB->GetUINT32Data( 5 );
				pPop->m_RecordSeason.m_ui32Win				= m_pDB->GetUINT32Data( 6 );
				pPop->m_RecordSeason.m_ui32Lose				= m_pDB->GetUINT32Data( 7 );

				pPop->m_RecordSeason.m_ui32Kill				= m_pDB->GetUINT32Data( 8 );
				pPop->m_RecordSeason.m_ui32Death			= m_pDB->GetUINT32Data( 9 );
				pPop->m_RecordSeason.m_ui32HeadShot			= m_pDB->GetUINT32Data( 10 );
				pPop->m_RecordSeason.m_ui32Assist			= m_pDB->GetUINT32Data( 11 );

				pPop->m_RecordSeason.m_ui32PlayTime			= m_pDB->GetUINT32Data( 13 );
				pPop->m_RecordSeason.m_ui32MVPCount			= m_pDB->GetUINT32Data( 14 );
				pPop->m_RecordSeason.m_ui32HelmetProtect	= m_pDB->GetUINT32Data( 15 );
				pPop->m_RecordSeason.m_ui32BestScore		= m_pDB->GetUINT32Data( 16 );
				pPop->m_RecordSeason.m_ui32BestDamage		= m_pDB->GetUINT32Data( 17 );

				pPop->m_RecordSeason.m_ui32RankPoint		= m_pDB->GetUINT32Data( 18 );
			}
			break;
		case DB_RESULT_EMPTYROW:			// 첫 접속 유저입니다.
			{	
				pPop->m_RecordSeason.Reset();
			}
			break;
		default:							// 알 수 없는 DB 에러
			{
				pPop->m_TResult				= NET_RESULT_ERROR_DB;
				return;
			}
			break;
		}
	}
	else
	{
		pPop->m_TResult				= NET_RESULT_ERROR;
		return;
	}

	pPop->m_Info.Reset();
	pPop->m_Eequipment.Reset();
}

void CDatabase::_SetUserInfo( RING_DB_PUSH* pPush, RING_DB_POP* pPop )
{
	pPop->m_TResult					= NET_RESULT_ERROR_DB;

	TCHAR strSQL[ S2_STRING_COUNT ];

	S2DB::MakeQuery( strSQL, S2_STRING_COUNT, _T("usp_UserInfo_mer"),
		S2DB_DATA_TYPE_BIGINT,					pPush->m_TUID,
		S2DB_DATA_TYPE_CHAR,					pPush->m_Base.m_strNickname,
		S2DB_DATA_TYPE_SMALLINT,				pPush->m_Base.m_ui16Level,
		S2DB_DATA_TYPE_INT,						pPush->m_Info.m_ui32Exp,
		S2DB_DATA_TYPE_END );
	if( m_pDB->ExecuteSP( strSQL ) && m_pDB->GetRecordCount())
	{
		INT32 i32Result = m_pDB->GetUINT32Data( 2 );
		if( DB_RESULT_SUCCESS == i32Result )
		{
			pPop->m_TResult = NET_RESULT_SUCCESS;
		}
	}
	
	S2DB::MakeQuery( strSQL, S2_STRING_COUNT, _T("usp_BattleRecord_mer"),
		S2DB_DATA_TYPE_BIGINT,					pPush->m_TUID,
		S2DB_DATA_TYPE_INT,						g_pConfig->m_ui32Season,
		S2DB_DATA_TYPE_INT,						pPush->m_Record.m_ui32Match,
		S2DB_DATA_TYPE_INT,						pPush->m_Record.m_ui32Win,
		S2DB_DATA_TYPE_INT,						pPush->m_Record.m_ui32Lose,
		S2DB_DATA_TYPE_INT,						pPush->m_Record.m_ui32Kill,
		S2DB_DATA_TYPE_INT,						pPush->m_Record.m_ui32Death,
		S2DB_DATA_TYPE_INT,						pPush->m_Record.m_ui32HeadShot,
		S2DB_DATA_TYPE_INT,						pPush->m_Record.m_ui32Assist,
		S2DB_DATA_TYPE_INT,						pPush->m_Record.m_ui32Kill,
		S2DB_DATA_TYPE_INT,						pPush->m_Record.m_ui32PlayTime,			//PlayTime
		S2DB_DATA_TYPE_INT,						pPush->m_Record.m_ui32MVPCount,			//MVP
		S2DB_DATA_TYPE_INT,						pPush->m_Record.m_ui32HelmetProtect,	//Helmet
		S2DB_DATA_TYPE_INT,						pPush->m_Record.m_ui32BestScore,		//BestScore
		S2DB_DATA_TYPE_INT,						pPush->m_Record.m_ui32BestDamage,		//BestDamage
		S2DB_DATA_TYPE_INT,						pPush->m_Record.m_ui32RankPoint,		//Rank
		S2DB_DATA_TYPE_END );

	if( m_pDB->ExecuteSP( strSQL ) && m_pDB->GetRecordCount())
	{
		INT32 i32Result = m_pDB->GetUINT32Data( 2 );
		if( DB_RESULT_SUCCESS == i32Result )
		{
			pPop->m_TResult = NET_RESULT_SUCCESS;
		}
	}
}

