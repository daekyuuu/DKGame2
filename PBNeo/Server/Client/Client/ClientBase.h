#pragma once
#include "NetworkTCP.h"

class CClientBase : public CFormView
{
	DECLARE_DYNCREATE(CClientBase)

public:
	CClientBase();
	CClientBase(UINT nIDTemplate);           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CClientBase();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

	CNetworkTCP*			m_pTCP;
public:
	void					SetTCP( CNetworkTCP* pTCP )			{	m_pTCP = pTCP;		}
};
