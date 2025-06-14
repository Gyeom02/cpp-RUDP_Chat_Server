#pragma once
#include "resource.h"
class CSignUpDialog : public CDialog
{

public:
    CSignUpDialog(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SIGNUP_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog() override;
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    virtual BOOL PreTranslateMessage(MSG* pMsg) override;
    DECLARE_MESSAGE_MAP()

public:
    CString m_strNickname;
    CString m_strUsername;
    CString m_strPassword;

    CEdit m_EditNickname;
    CEdit m_Editusername;

    afx_msg void OnBnClickedSignUp();
    afx_msg void OnBnClickedBack();
protected:
    CBrush m_bgBrush;

    CMFCButton m_btnBack;
    CMFCButton m_btnSignUp;
    CStatic m_ctrlError;

    
};

