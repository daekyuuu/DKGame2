#pragma once

#include "ClientBase.h"

// CClientLogin �� ���Դϴ�.

class CClientLogin : public CClientBase
{
	DECLARE_DYNCREATE(CClientLogin)

public:
	CClientLogin();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedLoginLogin();
	CString m_strID;
	CString m_strPW;
};


