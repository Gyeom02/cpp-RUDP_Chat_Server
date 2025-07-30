#pragma once
#include "resource.h"

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
	std::map<CString, std::vector<CString>> m_chatLogs;

	bool bStartedChat = false;

private:
	
};

