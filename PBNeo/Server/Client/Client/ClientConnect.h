#pragma once

#include "ClientBase.h"

// CClientConnect �� ���Դϴ�.

class CClientConnect : public CClientBase
{
	DECLARE_DYNCREATE(CClientConnect)

public:
	CClientConnect();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CClientConnect();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_CONNECT };
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

private:

	CIPAddressCtrl			m_ctIP;
	UINT					m_ui32Port;

public:
	virtual void			OnInitialUpdate();
	afx_msg void			OnBnClickedConnectConnect();
};


