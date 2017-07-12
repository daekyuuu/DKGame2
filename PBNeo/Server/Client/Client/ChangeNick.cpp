// ChangeNick.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Client.h"
#include "ChangeNick.h"
#include "afxdialogex.h"


// CChangeNick ��ȭ �����Դϴ�.

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


// CChangeNick �޽��� ó�����Դϴ�.


void CChangeNick::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData( TRUE );
	CDialogEx::OnOK();
}
