// ChangeNick.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Client.h"
#include "ChangeNick.h"
#include "afxdialogex.h"


// CChangeNick 대화 상자입니다.

IMPLEMENT_DYNAMIC(CChangeNick, CDialogEx)

CChangeNick::CChangeNick(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CHANGE_NICK, pParent)
	, m_strNickname(_T(""))
{

}

CChangeNick::~CChangeNick()
{
}

void CChangeNick::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CHANGENICK_NICK, m_strNickname);
}


BEGIN_MESSAGE_MAP(CChangeNick, CDialogEx)
	ON_BN_CLICKED(IDOK, &CChangeNick::OnBnClickedOk)
END_MESSAGE_MAP()


// CChangeNick 메시지 처리기입니다.


void CChangeNick::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );
	CDialogEx::OnOK();
}
