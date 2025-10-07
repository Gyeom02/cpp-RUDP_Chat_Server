#pragma once
#include "resource.h"
struct Friend
{
	Friend(int32 primid, string nickname) : _primid(primid), _nickname(nickname) {}
	int32 _primid;
	string _nickname;
};
class CMainDialog : public CDialog
{
public:
	
	CMainDialog(CWnd* pParent = nullptr);

	enum { IDD = IDD_DIALOG_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedLogout();
	afx_msg void OnBnClickedAddFriends();
	afx_msg void OnLbnSelchangeListFriends();
	afx_msg void OnBnClickedFriendRequest();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	DECLARE_MESSAGE_MAP()

public:
	void ReFreshFriendList();
	void AddFriends(vector<Friend>& friends);
	void AddFriend(Friend _friend);
	void SendGetFriends();
	string to_utf8(const std::wstring& wstr);
	CString utf8toCString(const std::string& wstr);
	int32 FindFriendIndex(int32 primid); // Using for to get listindex who gave me chat
	void HandleMSG(int32 handle, int32 listindex, string msg);
public:
	CListBox m_listFriends;
	CListBox m_listChatLog;
	CEdit m_editChatInput;
	CMFCButton m_btnSend;
	CMFCButton m_btnLogout;
	CMFCButton m_btnAddFriends;
	CMFCButton m_btnFriendRequets;
	CBrush m_bgBrush;
	CBrush m_darker;
	

	CString m_currentTarget;
	std::map<int32, std::vector<CString>> m_chatLogs;

	bool bStartedChat = false;

private:
	USE_LOCK;
	vector<Friend> _friends;
};

