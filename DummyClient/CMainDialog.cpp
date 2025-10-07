#include "pch.h"
#include "CMainDialog.h"
#include "CLoginDialog.h"
#include "CAddFriendsDialog.h"
#include "PlayerManager.h"
#include "CRequestDialog.h"
#include <codecvt>
#include <locale>

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

	user->SetCurDialog(this);

	SetDlgItemText(IDC_USER_NICKNAME, CString(user->GetNickName().c_str()));
	SetDlgItemText(IDC_USER_FRIENDCODE, CString(user->GetFriendCode().c_str()));

	m_bgBrush.CreateSolidBrush(COLOR_BACKGROUND);
	m_darker.CreateSolidBrush(COLOR_BACKGROUND_DARKER);

	
	SendGetFriends();
	

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

	user->Getx25519().generate();

	Protocol::C_SHAREPUBLICKEY pkt;
	std::vector<uint8> client_pub(32);
	x25519_get_public(user->Getx25519().get(), client_pub);
	pkt.set_primid(user->playerId);
	
	pkt.set_publickey(string(client_pub.begin(), client_pub.end()));

	auto SendBuffer = ServerPacketHandler::MakeReliableBuffer(pkt, QoSCore::HIGH);
	user->Send(SendBuffer);

	return TRUE;
}

void CMainDialog::OnBnClickedSend()
{
	CString strmessage;
	m_editChatInput.GetWindowText(strmessage);
	int32 friendindex = m_listFriends.GetCurSel();
	
	if (!strmessage.IsEmpty())
	{
		Protocol::C_SENDMSG sendpkt;
		sendpkt.set_from_id(to_string(user->playerId));
		sendpkt.set_to_id(to_string(_friends[friendindex]._primid));
		sendpkt.set_msg(string(to_utf8(wstring((strmessage)))));
		sendpkt.set_listindex(friendindex);
 		SendBufferRef sendbuffer = ServerPacketHandler::MakeReliableBuffer(sendpkt, QoSCore::HIGH);
		user->Send(sendbuffer);
		
		m_editChatInput.SetWindowText(_T(""));
		//TODO UDP Send 
	}
}

void CMainDialog::OnBnClickedLogout()
{
	if (AfxMessageBox(_T("로그아웃 하시겠습니까?"), MB_YESNO) == IDYES)
	{
		if (user)
		{
			if (user->playerId > 0)
			{
				Protocol::C_DISCONNECT pkt;
				pkt.set_id(user->playerId);
				pkt.set_roomid(user->roomId);
				pkt.set_roomprimid(user->roomprimid);
				SendBufferRef sendBuffer = ServerPacketHandler::MakeReliableBuffer(pkt, QoSCore::HIGH);

				user->Send(sendBuffer);
			}
		}

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

		if (m_chatLogs.count(nIndex))
		{
			for (const auto& msg : m_chatLogs[nIndex])
				m_listChatLog.AddString(msg);
		}
		else
		{
			Protocol::C_GETCHATLOG sendpkt;
			sendpkt.set_primid(to_string(user->playerId));
			sendpkt.set_to_id(to_string(_friends[nIndex]._primid));
			sendpkt.set_listindex(nIndex);
			SendBufferRef sendbuffer = ServerPacketHandler::MakeReliableBuffer(sendpkt, QoSCore::HIGH);
			user->Send(sendbuffer);
			/*CString welcome;
			welcome.Format(_T("%s 님과의 대화를 시작합니다."), m_currentTarget);
			m_listChatLog.AddString(welcome);*/
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

void CMainDialog::ReFreshFriendList()
{
	m_listFriends.ResetContent();
	for (int i = 0; i < _friends.size(); i++)
	{
		m_listFriends.AddString(CString(_friends[i]._nickname.c_str()));
	}
}

void CMainDialog::AddFriends(vector<Friend>& friends)
{
	WRITE_LOCK;
	_friends = friends;
}

void CMainDialog::AddFriend(Friend _friend)
{
	WRITE_LOCK;
	_friends.push_back(_friend);
}

void CMainDialog::SendGetFriends()
{
	Protocol::C_GETFRIENDS sendpkt;
	sendpkt.set_primid(to_string(user->playerId));
	SendBufferRef sendbuffer = ServerPacketHandler::MakeReliableBuffer(sendpkt, QoSCore::HIGH);
	user->Send(sendbuffer);
}

string CMainDialog::to_utf8(const std::wstring& wstr)
{
	if (wstr.empty()) return {};
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), strTo.data(), size_needed, nullptr, nullptr);
	return strTo;
}
CString CMainDialog::utf8toCString(const std::string& str)
{
	if (str.empty())
		return CString();

	// 1. UTF-8 → wstring
	int wlen = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0);
	std::wstring wstr(wlen, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), &wstr[0], wlen);

	// 2. wstring → CString
	return CString(wstr.c_str());
}
int32 CMainDialog::FindFriendIndex(int32 primid)
{
	for (int32 i = 0 ; i < _friends.size(); i ++)
	{
		auto& f = _friends[i];
		if (f._primid == primid)
		{
			return i;
		}
	}
	return -1;
}

void CMainDialog::HandleMSG(int32 handle, int32 listindex, string msg)
{
	if (handle == 0) // 자신이 보낸 채팅이라는 뜻
	{
		CString logEntry = _T("나: ") + utf8toCString(msg);

		m_chatLogs[listindex].push_back(logEntry);
		m_listChatLog.AddString(logEntry);
		
	}
	else // 상대방의 채팅이라는 뜻
	{		
		CString strFriend;
		m_listFriends.GetText(listindex, strFriend);
		CString logEntry = strFriend + _T(": ") + utf8toCString(msg);

		m_chatLogs[listindex].push_back(logEntry);
		if(m_listFriends.GetCurSel() == listindex)
			m_listChatLog.AddString(logEntry);
	}
}

