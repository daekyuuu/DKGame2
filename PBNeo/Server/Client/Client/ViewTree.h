
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CViewTree â�Դϴ�.

class CViewTree : public CTreeCtrl
{
// �����Դϴ�.
public:
	CViewTree();

// �������Դϴ�.
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// �����Դϴ�.
public:
	virtual ~CViewTree();

protected:
	DECLARE_MESSAGE_MAP()
};
