#include "pch.h"
#include "CMainDialog.h"
#include "CLoginDialog.h"
#include "CAddFriendsDialog.h"
#include "PlayerManager.h"
#include "CRequestDialog.h"

BEGIN_MESSAGE_MAP(CMainDialog, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CMainDialog::OnBnClickedSend)
	ON_BN_CLICKED(IDC_BUTTON_LOGOUT, &CMainDialog::OnBnClickedLogout)
	ON_BN_CLICKED(IDC_BUTTON_ADDFRIENDS, &CMainDialog::OnBnClickedAddFriends)
	ON_BN_CLICKED(IDC_BTN_FRIEND_REQUEST, & CMainDialog::OnBnClickedFriendRequest)
	ON_LBN_SELCHANGE(IDC_LIST_FRIENDS, &CMainDialog::OnLbnSelchangeListFriends)

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

	SetDlgItemText(IDC_USER_NICKNAME, CString(user.GetNickName().c_str()));

	m_bgBrush.CreateSolidBrush(COLOR_BACKGROUND);
	m_darker.CreateSolidBrush(COLOR_BACKGROUND_DARKER);


	m_listFriends.AddString(_T("ģ�� 1"));
	m_listFriends.AddString(_T("ģ�� 2"));
	

	m_btnLogout.SubclassDlgItem(IDC_BUTTON_LOGOUT, this);
	m_btnLogout.SetFaceColor(COLOR_BUTTON, TRUE);
	m_btnLogout.SetTextColor(RGB(255, 255, 255));

	m_btnSend.SubclassDlgItem(IDC_BUTTON_SEND, this);
	m_btnSend.SetFaceColor(COLOR_DARKER_BUTTON, TRUE);
	m_btnSend.SetTextColor(RGB(255, 255, 255));

	m_btnAddFriends.SubclassDlgItem(IDC_BUTTON_ADDFRIENDS, this);
	m_btnAddFriends.SetFaceColor(COLOR_BUTTON, TRUE);
	m_btnAddFriends.SetTextColor(RGB(255, 255, 255));

	
	m_btnFriendRequets.SubclassDlgItem(IDC_BTN_FRIEND_REQUEST, this);
	m_btnFriendRequets.SetFaceColor(COLOR_BUTTON, TRUE);
	m_btnFriendRequets.SetTextColor(RGB(255, 255, 255));

	GetDlgItem(IDC_EDIT_CHATINPUT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_SEND)->ShowWindow(SW_HIDE);
	return TRUE;
}

void CMainDialog::OnBnClickedSend()
{
	CString strmessage;
	m_editChatInput.GetWindowText(strmessage);
	
	
	if (!strmessage.IsEmpty())
	{
		CString logEntry = _T("��: ") + strmessage;

		m_chatLogs[m_currentTarget].push_back(logEntry);
		m_listChatLog.AddString(logEntry);
		m_editChatInput.SetWindowText(_T(""));

		//TODO UDP Send 
	}
}

void CMainDialog::OnBnClickedLogout()
{
	if (AfxMessageBox(_T("�α׾ƿ� �Ͻðڽ��ϱ�?"), MB_YESNO) == IDYES)
	{
		CLoginDialog dlg;
		AfxGetApp()->m_pMainWnd = &dlg;
		this->ShowWindow(SW_HIDE);
		dlg.DoModal();
		this->ShowWindow(SW_SHOW);
		EndDialog(IDOK);
	}
}

void CMainDialog::OnBnClickedAddFriends()
{
	CAddFriendsDialog dlg;
	AfxGetApp()->m_pMainWnd = &dlg;
	dlg.ownerDialog = this;
	dlg.DoModal();
	
}

void CMainDialog::OnLbnSelchangeListFriends()
{
	int nIndex = m_listFriends.GetCurSel();
	if (nIndex != LB_ERR)
	{
		if (!bStartedChat)
		{
			bStartedChat = true;
			GetDlgItem(IDC_EDIT_CHATINPUT)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_SEND)->ShowWindow(SW_SHOW);
		}
		CString strFriend;
		m_listFriends.GetText(nIndex, strFriend);
		m_currentTarget = strFriend;

		m_listChatLog.ResetContent();

		if (m_chatLogs.count(m_currentTarget))
		{
			for (const auto& msg : m_chatLogs[m_currentTarget])
				m_listChatLog.AddString(msg);
		}
		else
		{
			CString welcome;
			welcome.Format(_T("%s �԰��� ��ȭ�� �����մϴ�."), m_currentTarget);
			m_listChatLog.AddString(welcome);
		}
	}
}

void CMainDialog::OnBnClickedFriendRequest()
{
	CRequestDialog dlg;
	AfxGetApp()->m_pMainWnd = &dlg;
	dlg.ownerDlg = this;
	dlg.DoModal();
}

HBRUSH CMainDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	int nID = pWnd->GetDlgCtrlID();

	// ��ü �ؽ�Ʈ ���� �⺻ ����
	pDC->SetTextColor(RGB(220, 221, 222));
	pDC->SetBkMode(TRANSPARENT);

	if (pWnd->GetSafeHwnd() == m_listFriends.GetSafeHwnd())
	{
		pDC->SetBkColor(RGB(0, 0, 0)); // ��� ������
		pDC->SetTextColor(RGB(255, 255, 255)); // ���� �Ͼ��(������)
		return (HBRUSH)m_darker.GetSafeHandle();
	}
	if (pWnd->GetSafeHwnd() == m_listChatLog.GetSafeHwnd())
	{
		pDC->SetBkColor(RGB(0, 0, 0)); // ��� ������
		pDC->SetTextColor(RGB(255, 255, 255)); // ���� �Ͼ��(������)
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

