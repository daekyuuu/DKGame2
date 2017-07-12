// ClientConnect.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Client.h"
#include "ClientBase.h"


// CClientBase

IMPLEMENT_DYNCREATE(CClientBase, CFormView)

CClientBase::CClientBase()
	: CFormView((UINT)0)
{
}

CClientBase::CClientBase(UINT nIDTemplate)
	: CFormView(nIDTemplate)
{

}

CClientBase::~CClientBase()
{
}

void CClientBase::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClientBase, CFormView)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CClientBase 진단입니다.

#ifdef _DEBUG
void CClientBase::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CClientBase::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

