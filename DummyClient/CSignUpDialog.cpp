#include "pch.h"
#include "CSignUpDialog.h"
#include "CLoginDialog.h"

BEGIN_MESSAGE_MAP(CSignUpDialog, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_DO_SIGNUP, &CSignUpDialog::OnBnClickedSignUp)
	ON_BN_CLICKED(IDC_BUTTON_BACK, &CSignUpDialog::OnBnClickedBack)

END_MESSAGE_MAP()


CSignUpDialog::CSignUpDialog(CWnd* pParent)
	:CDialog(IDD_SIGNUP_DIALOG, pParent)
{
}



void CSignUpDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_strUsername);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_EDIT_NICKNAME, m_strNickname);
	
}

BOOL CSignUpDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bgBrush.CreateSolidBrush(COLOR_BACKGROUND);
	
	m_ctrlError.SubclassDlgItem(IDC_STATIC_SIGNUP_ERROR, this);
	m_ctrlError.SetWindowTextW(_T(""));
	
	m_btnBack.SubclassDlgItem(IDC_BUTTON_LOGIN, this);
	m_btnBack.SetFaceColor(COLOR_BUTTON, TRUE);
	m_btnBack.SetTextColor(RGB(255, 255, 255));

	m_btnSignUp.SubclassDlgItem(IDC_BUTTON_SIGNUP, this);
	m_btnSignUp.SetFaceColor(COLOR_BUTTON, TRUE);
	m_btnSignUp.SetTextColor(RGB(255, 255, 255));


	return TRUE;
}

void CSignUpDialog::OnBnClickedBack()
{
	if (AfxMessageBox(_T("���ư��ðڽ��ϱ�?"), MB_YESNO) == IDYES)
	{
		CLoginDialog dlg;
		this->ShowWindow(SW_HIDE);
		dlg.DoModal();
		this->ShowWindow(SW_SHOW);
		EndDialog(IDOK);
	}
}

void CSignUpDialog::OnBnClickedSignUp()
{
	UpdateData(TRUE);

	if (m_strNickname.IsEmpty() || m_strUsername.IsEmpty() || m_strPassword.IsEmpty())
	{
		m_ctrlError.SetWindowTextW(_T("�������� �Է����ּ���"));
		return;
	}

	//if (m_strID == _T("admin") && m_strPassword == _T("pass"))
	//{
	//	CMainDialog dlg;
	//	this->ShowWindow(SW_HIDE);
	//	dlg.DoModal();
	//	this->ShowWindow(SW_SHOW);
	//	EndDialog(IDOK);
	//}
	//else
	//{
	//	m_ctrlError.SetWindowTextW(_T("�α��� ���� : �ùٸ��� ���� ID �Ǵ� ��й�ȣ"));
	//}

	if (AfxMessageBox(_T("�����Ͻðڽ��ϱ�?"), MB_YESNO) == IDYES)
	{
		//TODO 

		
		CLoginDialog dlg;
		this->ShowWindow(SW_HIDE);
		dlg.DoModal();
		this->ShowWindow(SW_SHOW);
		EndDialog(IDOK);
	}
}

HBRUSH CSignUpDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	int nID = pWnd->GetDlgCtrlID();

	// ��ü �ؽ�Ʈ ���� �⺻ ����
	pDC->SetTextColor(RGB(220, 221, 222));
	pDC->SetBkMode(TRANSPARENT);

	if (GetDlgItem(IDC_STATIC_SIGNUP_ERROR)->m_hWnd == pWnd->m_hWnd)
	{
		pDC->SetTextColor(RGB(255, 0, 0));
		return m_bgBrush;
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

BOOL CSignUpDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		OnBnClickedSignUp();
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

