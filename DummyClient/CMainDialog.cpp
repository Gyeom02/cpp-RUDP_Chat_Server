#include "pch.h"
#include "CMainDialog.h"
#include "CLoginDialog.h"
#include "CAddFriendsDialog.h"

BEGIN_MESSAGE_MAP(CMainDialog, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CMainDialog::OnBnClickedSend)
	ON_BN_CLICKED(IDC_BUTTON_LOGOUT, &CMainDialog::OnBnClickedLogout)
	ON_BN_CLICKED(IDC_BUTTON_ADDFRIENDS, &CMainDialog::OnBnClickedAddFriends)

END_MESSAGE_MAP()


CMainDialog::CMainDialog(CWnd* pParent)
	:CDialog(IDD_DIALOG_MAIN, pParent)
{
}


void CMainDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FRIENDS, m_listFriends);
	DDX_Control(pDX, IDC_LIST_CHATLOG, m_listChatLog);
	DDX_Control(pDX, IDC_EDIT_CHATINPUT, m_editChatInput);
	/*DDX_Control(pDX, IDC_BUTTON_SEND, m_btnSend);
	DDX_Control(pDX, IDC_BUTTON_LOGOUT, m_btnLogout);
	DDX_Control(pDX, IDC_BUTTON_ADDFRIENDS, m_btnAddFriends);*/
}

BOOL CMainDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bgBrush.CreateSolidBrush(COLOR_BACKGROUND);
	m_darker.CreateSolidBrush(COLOR_BACKGROUND_DARKER);


	m_listFriends.AddString(_T("친구 1"));
	m_listFriends.AddString(_T("친구 2"));
	

	m_btnLogout.SubclassDlgItem(IDC_BUTTON_LOGOUT, this);
	m_btnLogout.SetFaceColor(COLOR_BUTTON, TRUE);
	m_btnLogout.SetTextColor(RGB(255, 255, 255));

	m_btnSend.SubclassDlgItem(IDC_BUTTON_SEND, this);
	m_btnSend.SetFaceColor(RGB(39, 174, 96), TRUE);
	m_btnSend.SetTextColor(RGB(255, 255, 255));

	m_btnAddFriends.SubclassDlgItem(IDC_BUTTON_ADDFRIENDS, this);
	m_btnAddFriends.SetFaceColor(COLOR_BUTTON, TRUE);
	m_btnAddFriends.SetTextColor(RGB(255, 255, 255));


	return TRUE;
}

void CMainDialog::OnBnClickedSend()
{
	CString strmessage;
	m_editChatInput.GetWindowText(strmessage);
	
	if (!strmessage.IsEmpty())
	{
		CString logEntry = _T("나: ") + strmessage;
		m_listChatLog.AddString(logEntry);
		m_editChatInput.SetWindowText(_T(""));
	}
}

void CMainDialog::OnBnClickedLogout()
{
	if (AfxMessageBox(_T("로그아웃 하시겠습니까?"), MB_YESNO) == IDYES)
	{
		CLoginDialog dlg;
		this->ShowWindow(SW_HIDE);
		dlg.DoModal();
		this->ShowWindow(SW_SHOW);
		EndDialog(IDOK);
	}
}

void CMainDialog::OnBnClickedAddFriends()
{
	CAddFriendsDialog dlg;
	dlg.DoModal();
	
}

HBRUSH CMainDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	int nID = pWnd->GetDlgCtrlID();

	// 전체 텍스트 색상 기본 적용
	pDC->SetTextColor(RGB(220, 221, 222));
	pDC->SetBkMode(TRANSPARENT);

	if (pWnd->GetSafeHwnd() == m_listFriends.GetSafeHwnd())
	{
		pDC->SetBkColor(RGB(0, 0, 0)); // 배경 검은색
		pDC->SetTextColor(RGB(255, 255, 255)); // 글자 하얀색(가독성)
		return (HBRUSH)m_darker.GetSafeHandle();
	}
	if (pWnd->GetSafeHwnd() == m_listChatLog.GetSafeHwnd())
	{
		pDC->SetBkColor(RGB(0, 0, 0)); // 배경 검은색
		pDC->SetTextColor(RGB(255, 255, 255)); // 글자 하얀색(가독성)
		return (HBRUSH)m_bgBrush.GetSafeHandle();
	}

	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
		return m_bgBrush;
	case CTLCOLOR_STATIC:
		return m_bgBrush;
	case CTLCOLOR_BTN:
		return m_bgBrush;
	case CTLCOLOR_EDIT:
		pDC->SetBkColor(RGB(62, 66, 70));
		return CreateSolidBrush(RGB(62, 66, 70));
	}
	return hbr;
}

BOOL CMainDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		OnBnClickedSend();
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

