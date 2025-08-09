#pragma once
#include "resource.h"

struct Request
{
	Request(int32 id, string nickname) : _id(id), _nickname(nickname) {}
	int32 _id;
	string _nickname;
};

class CRequestDialog : public CDialog
{
	
public:
	CRequestDialog(CWnd* pParent = nullptr);
	~CRequestDialog();
	enum { IDD = IDD_FRIENDREQUESTS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog() override;
	
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	//void SetError(int32 i);
	DECLARE_MESSAGE_MAP()

	//	HBRUSH m_brushBackground;
	CBrush m_bgBrush;
	CBrush m_darker;
	

public:
	CMFCButton m_btnAccept;
	CMFCButton m_btnReject;
	CListBox m_listFriendRequests;

	//CStatic m_ctrlError;

	//COLORREF m_errorColor;

public:
	afx_msg void OnBnClickedAccept();
	afx_msg void OnBnClickedReject();
	afx_msg void OnCancel();

	void FreshRequestsList();
	void AddRequest(int32 id, string name);
	void AddRequests(vector<Request>& request);
	void ApplyRequestPkt(int32 index, int32 bsuccess);
public:
	CDialog* ownerDlg = nullptr;
private:
	vector<Request> _requests;
	USE_LOCK;
	//int32 curRequestindex; // 업데이트 된 최근 vector의 끝 수 
};

