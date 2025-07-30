#pragma once
#include "resource.h"

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
public:

	CDialog* ownerDlg = nullptr;
};

