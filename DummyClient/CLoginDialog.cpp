#include "pch.h"
#include "CLoginDialog.h"
#include "CMainDialog.h"
#include "CSignUpDialog.h"
#include "CFindAccDialog.h"

BEGIN_MESSAGE_MAP(CLoginDialog, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CLoginDialog::OnBnClickedLogin)
	ON_BN_CLICKED(IDC_BUTTON_SIGNUP, &CLoginDialog::OnBnClickedSignup)
	ON_BN_CLICKED(IDC_BUTTON_FIND, &CLoginDialog::OnBnClickedFind)

END_MESSAGE_MAP()

CLoginDialog::CLoginDialog(CWnd* pParent)
	: CDialog(IDD_LOGIN_DIALOG, pParent)
{

}

void CLoginDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ID, m_strID);
	DDX_Text(pDX, IDC_EDIT_PW, m_strPassword);

}

BOOL CLoginDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctrlError.SubclassDlgItem(IDC_STATIC_ERROR, this);
	m_ctrlError.SetWindowTextW(_T(""));

	m_bgBrush.CreateSolidBrush(COLOR_BACKGROUND);


	m_btnLogin.SubclassDlgItem(IDC_BUTTON_LOGIN, this);
	m_btnLogin.SetFaceColor(COLOR_BUTTON, TRUE);
	m_btnLogin.SetTextColor(RGB(255, 255, 255));
	
	m_btnSignup.SubclassDlgItem(IDC_BUTTON_SIGNUP, this);
	m_btnSignup.SetFaceColor(RGB(39, 174, 96), TRUE);
	m_btnSignup.SetTextColor(RGB(255, 255, 255));

	m_btnFind.SubclassDlgItem(IDC_BUTTON_FIND, this);
	m_btnFind.SetFaceColor(RGB(39, 174, 96), TRUE);
	m_btnFind.SetTextColor(RGB(255, 255, 255));

	return TRUE;
}

HBRUSH CLoginDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	int nID = pWnd->GetDlgCtrlID();

	// 전체 텍스트 색상 기본 적용
	pDC->SetTextColor(RGB(220, 221, 222));
	pDC->SetBkMode(TRANSPARENT);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_ERROR)  // 대상 Static ID
		{
			pDC->SetTextColor(RGB(255,0,0));
			pDC->SetBkMode(TRANSPARENT);
			return m_bgBrush;
		}
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_TITLE)
		{
			pDC->SetTextColor(COLOR_BUTTON);
			pDC->SetBkMode(TRANSPARENT);
			return m_bgBrush;
		}
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

BOOL CLoginDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		OnBnClickedLogin();
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CLoginDialog::OnBnClickedLogin()
{
	UpdateData(TRUE);

	if (m_strID.IsEmpty() || m_strPassword.IsEmpty())
	{
		m_ctrlError.SetWindowTextW(_T("ID 또는 비밀번호를 입력해주세요."));
		return;
	}

	if (m_strID == _T("admin") && m_strPassword == _T("pass"))
	{
		CMainDialog dlg;
		this->ShowWindow(SW_HIDE);
		dlg.DoModal();
		this->ShowWindow(SW_SHOW);
		EndDialog(IDOK);
	}
	else
	{
		m_ctrlError.SetWindowTextW(_T("로그인 실패 : 올바르지 않은 ID 또는 비밀번호"));
	}
}

void CLoginDialog::OnBnClickedSignup()
{
	CSignUpDialog dlg;
	this->ShowWindow(SW_HIDE);
	dlg.DoModal();
	this->ShowWindow(SW_SHOW);
	EndDialog(IDOK);
}

void CLoginDialog::OnBnClickedFind()
{
	AfxMessageBox(_T("개발중입니다"));
	/*CFindAccDialog dlg;
	this->ShowWindow(SW_HIDE);
	dlg.DoModal();
	this->ShowWindow(SW_SHOW);
	EndDialog(IDOK);*/
}

