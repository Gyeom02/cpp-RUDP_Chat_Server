#pragma once
#include "resource.h"

class CAddFriendsDialog : public CDialog
{

public:
	CAddFriendsDialog(CWnd* pParent = nullptr);
	enum { IDD = IDD_ADDFRIENDS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog() override;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	void SetError(int32 i);
	DECLARE_MESSAGE_MAP()

	//	HBRUSH m_brushBackground;
	CBrush m_bgBrush;

	CMFCButton m_btnAdd;
	
private:
	CEdit m_strFriendsText;
	
	CStatic m_ctrlError;

	COLORREF m_errorColor;

public:
	afx_msg void OnBnClickedAdd();
	afx_msg void OnCancel();

	CDialog* ownerDialog;
};

