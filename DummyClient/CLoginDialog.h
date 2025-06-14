#pragma once

#include "resource.h"

class CLoginDialog : public CDialog
{
public:
	CLoginDialog(CWnd* pParent = nullptr);
	enum { IDD = IDD_LOGIN_DIALOG};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog() override;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	DECLARE_MESSAGE_MAP()
	
//	HBRUSH m_brushBackground;
	CBrush m_bgBrush;

	CMFCButton m_btnLogin;
	CMFCButton m_btnSignup;
	CMFCButton m_btnFind;
	
private:
	CString m_strID;
	CString m_strPassword;
	CStatic m_ctrlError;

	

public:
	afx_msg void OnBnClickedLogin();
	afx_msg void OnBnClickedSignup();
	afx_msg void OnBnClickedFind();
	CString GetID() const { return m_strID; }
	CString GetPassword() const { return m_strPassword;  }
};

