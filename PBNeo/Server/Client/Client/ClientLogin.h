#pragma once

#include "ClientBase.h"

// CClientLogin 폼 뷰입니다.

class CClientLogin : public CClientBase
{
	DECLARE_DYNCREATE(CClientLogin)

public:
	CClientLogin();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CClientLogin();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_LOGIN };
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
public:
	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedLoginLogin();
	CString m_strID;
	CString m_strPW;
};


