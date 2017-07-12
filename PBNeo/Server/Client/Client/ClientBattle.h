#pragma once

#include "ClientBase.h"
#include "afxcmn.h"

// CClientBattle 폼 뷰입니다.

class CClientBattle : public CClientBase
{
	DECLARE_DYNCREATE(CClientBattle)

public:
	CClientBattle();           // 동적 만들기에 사용되는 protected 생성자입니다.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
protected:	

public:
	virtual void			OnInitialUpdate();

	
	afx_msg void OnBnClickedBattleLeave();
};


