#include "pch.h"
#include "CAddFriendsDialog.h"
#include "CMainDialog.h"

BEGIN_MESSAGE_MAP(CAddFriendsDialog, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_CONFIRM_ADD, &CAddFriendsDialog::OnBnClickedAdd)
	

END_MESSAGE_MAP()

CAddFriendsDialog::CAddFriendsDialog(CWnd* pParent)
	: CDialog(IDD_ADDFRIENDS_DIALOG, pParent)
{

}

void CAddFriendsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NEW_FRIEND_NAME, m_strFriendsText);
	
}

BOOL CAddFriendsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	user->SetCurDialog(this);

	m_ctrlError.SubclassDlgItem(IDC_ADDFRIENDS_ERROR, this);
	m_ctrlError.SetWindowTextW(_T(""));

	m_bgBrush.CreateSolidBrush(COLOR_BACKGROUND);


	m_btnAdd.SubclassDlgItem(IDC_BUTTON_CONFIRM_ADD, this);
	m_btnAdd.SetFaceColor(COLOR_BUTTON, TRUE);
	m_btnAdd.SetTextColor(RGB(255, 255, 255));


	return TRUE;
}

HBRUSH CAddFriendsDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	int nID = pWnd->GetDlgCtrlID();

	// 전체 텍스트 색상 기본 적용
	pDC->SetTextColor(RGB(220, 221, 222));
	pDC->SetBkMode(TRANSPARENT);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		if (pWnd->GetDlgCtrlID() == IDC_ADDFRIENDS_ERROR)  // 대상 Static ID
		{
			pDC->SetTextColor(m_errorColor);
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

BOOL CAddFriendsDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		OnBnClickedAdd();
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CAddFriendsDialog::SetError(int32 i)
{
	if (i == 0) // Set Green
	{
		m_errorColor = COLOR_BUTTON;
	}
	if (i == 2)
	{
		m_errorColor = RGB(0, 0, 255);
	}
	else  // SetRed
	{
		m_errorColor = RGB(255, 0, 0);
	}
	Invalidate();
}


void CAddFriendsDialog::OnBnClickedAdd()
{
	UpdateData(TRUE);

	CString strmessage;
	m_strFriendsText.GetWindowText(strmessage);

	if (strmessage.IsEmpty())
	{
		SetError(1);
		m_ctrlError.SetWindowTextW(_T("친구코드를 입력하세요"));
		return;
	}
	
	//TODO Check Server if this Friends Code Exist or Not
	
	SetError(2);
	m_ctrlError.SetWindowTextW(_T("보내는 중"));
	m_strFriendsText.SetWindowTextW(_T(""));

	Protocol::C_REQUESTFRIEND sendpkt;
	std::string id = to_string(user->playerId);
	sendpkt.set_primid(id);
	sendpkt.set_friendcode(CT2CA(strmessage));

	SendBufferRef sendBuffer = ServerPacketHandler::MakeReliableBuffer(sendpkt, QoSCore::HIGH);

	user->Send(sendBuffer);
}


void CAddFriendsDialog::OnCancel()
{
	CMainDialog* mainDlg = dynamic_cast<CMainDialog*>(ownerDialog);
	if (mainDlg)
		AfxGetApp()->m_pMainWnd = mainDlg;
	CDialog::OnCancel();
}

void CAddFriendsDialog::ResponseAddFriend(int32 code)
{
	if (code == 0)
	{
		SetError(1);
		m_ctrlError.SetWindowTextW(_T("친구코드가 정확하지 않습니다"));
		return;
	}
	else if (code == 1)
	{
		SetError(0);
		m_ctrlError.SetWindowTextW(_T("성공적으로 보냈습니다"));
		return;
	}
}
