#pragma once


// CChangeNick 대화 상자입니다.

class CChangeNick : public CDialogEx
{
	DECLARE_DYNAMIC(CChangeNick)

public:
	CChangeNick(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CChangeNick();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHANGE_NICK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strNickname;
	afx_msg void OnBnClickedOk();
};
