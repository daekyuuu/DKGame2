#pragma once

#include "ClientBase.h"
#include "afxcmn.h"

// CClientBattle �� ���Դϴ�.

class CClientBattle : public CClientBase
{
	DECLARE_DYNCREATE(CClientBattle)

public:
	CClientBattle();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CClientBattle();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_BATTLE };
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
protected:	

public:
	virtual void			OnInitialUpdate();

	
	afx_msg void OnBnClickedBattleLeave();
};


