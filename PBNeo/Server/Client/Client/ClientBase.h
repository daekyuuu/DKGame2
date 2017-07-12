#pragma once
#include "NetworkTCP.h"

class CClientBase : public CFormView
{
	DECLARE_DYNCREATE(CClientBase)

public:
	CClientBase();
	CClientBase(UINT nIDTemplate);           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CClientBase();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	CNetworkTCP*			m_pTCP;
public:
	void					SetTCP( CNetworkTCP* pTCP )			{	m_pTCP = pTCP;		}
};
