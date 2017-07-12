#ifndef __S2_DB_ADO_H__
#define __S2_DB_ADO_H__

#pragma warning( disable : 4251 )  // Disable warning messages
//#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF")
//#import ".\msado60_Backcompat_ia64.tlb" no_namespace rename("EOF", "adoEOF")	// AMD
//#import ".\msado60_Backcompat_i386.tlb" no_namespace rename("EOF", "adoEOF")	// Intel x86
#import ".\msado60_Backcompat_x64.tlb" no_namespace rename("EOF", "adoEOF")		// Intel x64

#include <ATLComTime.h>

/*-----------------------------------------------------------------------
Name : S2DBAdo
2007 03 06 : ¹ö±×È®ÀÎ : GetComError(_com_error& err, _ConnectionPtr pConn) <- ÇÈ½ºµÊ

-----------------------------------------------------------------------*/
struct S2_EXPORT_BASE S2DB_CONNECT_INFO
{
	TCHAR					m_strDataSource[ S2_STRING_COUNT ];
	TCHAR					m_strCatalog[ S2_STRING_COUNT ];
	TCHAR					m_strID[ S2_STRING_COUNT ];
	TCHAR					m_strPW[ S2_STRING_COUNT ];
};

class S2_EXPORT_BASE S2DBAdo
{
protected:
	_ConnectionPtr			m_pConnection;		// ADO Connetion °´Ã¼
	_RecordsetPtr			m_pRecordSet;		// ADO RecordSet °´Ã¼
    //_CommandPtr			m_pCommand;			// ADO Command °´Ã¼

	long					m_nRecordCount;

	BOOL					m_bConnection;		// Mess Å©·¡½¬·Î ÀÎÇÑ ¶«»§ÄÚµåÀÔ´Ï´Ù.
	//char 					m_strError[SERVER_STRING_COUNT];

	BOOL					m_bCursorLocation_adUserClientFlag; // CursorLocation Type ¼³Á¤¿¡ ´ëÇÑ Flag.
	TCHAR					m_strLastError[ S2_STRING_COUNT ];
	HRESULT					m_hrLastError;

public:
	S2DBAdo();
	virtual ~S2DBAdo();

	BOOL					Create(BOOL bCurserTypeFlag = FALSE);

	BOOL IsConnected(void)
	{
		if (m_pConnection)
		{
			if (m_pConnection->GetState() == adStateOpen) return TRUE;
		}
		return FALSE;
	}

	//LPCSTR			GetConnenctionVersion();

	BOOL					BeginTrans();
	BOOL					CommitTrans();
	BOOL					RollbackTrans();

	BOOL					Close(void);
	BOOL					IsEOF(void);
	BOOL					Connect(TCHAR * strConnectFile);
	BOOL					Connect( S2DB_CONNECT_INFO* pConnectInfo );
	BOOL					ReConnect(TCHAR * strConnectFile);
	BOOL					ReConnect( S2DB_CONNECT_INFO* pConnectInfo );
	BOOL					Execute(TCHAR * strSql);
	BOOL					ExecuteQuery(TCHAR * strSql, BOOL IsLock = TRUE);
	BOOL					ExecuteSP(TCHAR * strSql);

	BOOL					MoveFirst(void);
	BOOL					Move(long nMove);
	BOOL					MoveNext(void);

	long					GetRecordCount();	   
	BOOL					GetboolData(long nFieldIndex);

	INT8					GetINT8Data(long nFieldIndex); 
	UINT8					GetUINT8Data(long nFieldIndex); 
	INT16					GetINT16Data(long nFieldIndex);
	UINT16					GetUINT16Data(long nFieldIndex);
	INT32					GetINT32Data(long nFieldIndex);
	UINT32					GetUINT32Data(long nFieldIndex);
	INT64					GetINT64Data(long nFieldIndex);
	UINT64					GetUINT64Data(long nFieldIndex);

	long					GetLongData(long nFieldIndex);
	float					GetFloatData(long nFieldIndex);
	DATE					GetDateData(long nFieldIndex);
	double					GetDoubleData(long nFieldIndex);

	void					GetTextData(long nFieldIndex, TCHAR * pString, INT32 Maxlen);
	void					GetBinaryData(long nFieldIndex, void * pData, INT32 Maxlen);

	BOOL					SetINT32Data(long nFieldIndex, INT32 data);
	BOOL					SetINT64Data(long nFieldIndex, INT64 data);
	BOOL					SetBinaryData( long nFieldIndex, void * pData, INT32 Length ); 

	void					GetComError(_com_error& err, _ConnectionPtr pConn);

	_RecordsetPtr			GetRecordSet() { return m_pRecordSet; }

	TCHAR*					GetLastError()					{	return m_strLastError;	}
	HRESULT					GethrLastError()				{	return m_hrLastError;	}
	bool					IsNeedConnect();
};

#endif