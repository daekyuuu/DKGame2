
// CServerApp.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CServerApp:
// �� Ŭ������ ������ ���ؼ��� ServerApp.cpp�� �����Ͻʽÿ�.
//

class CServerApp : public CWinApp
{
public:
	CServerApp();
	
// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CServerApp theApp;