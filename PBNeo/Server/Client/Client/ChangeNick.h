#pragma once


// CChangeNick ��ȭ �����Դϴ�.

class CChangeNick : public CDialogEx
{
	DECLARE_DYNAMIC(CChangeNick)

public:
	CChangeNick(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CChangeNick();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHANGE_NICK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strNickname;
	afx_msg void OnBnClickedOk();
};
