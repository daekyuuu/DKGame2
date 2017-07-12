// ClientLoadout.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Client.h"
#include "ClientLoadout.h"


// CClientLoadout

IMPLEMENT_DYNCREATE(CClientLoadout, CClientBase)

CClientLoadout::CClientLoadout()
	: CClientBase(IDD_CLIENT_LOADOUT)
{

}

CClientLoadout::~CClientLoadout()
{
}

void CClientLoadout::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClientLoadout, CFormView)
END_MESSAGE_MAP()


// CClientLoadout 진단입니다.

#ifdef _DEBUG
void CClientLoadout::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CClientLoadout::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CClientLoadout 메시지 처리기입니다.
