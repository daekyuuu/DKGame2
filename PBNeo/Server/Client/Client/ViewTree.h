
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CViewTree 창입니다.

class CViewTree : public CTreeCtrl
{
// 생성입니다.
public:
	CViewTree();

// 재정의입니다.
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// 구현입니다.
public:
	virtual ~CViewTree();

protected:
	DECLARE_MESSAGE_MAP()
};
