#pragma once

#include "ClientBase.h"

// CClientLoadout �� ���Դϴ�.

class CClientLoadout : public CClientBase
{
	DECLARE_DYNCREATE(CClientLoadout)

public:
	CClientLoadout();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CClientLoadout();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_LOADOUT };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};


