#include "pch.h"
#include "CRequestDialog.h"
#include "CMainDialog.h"

BEGIN_MESSAGE_MAP(CRequestDialog, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_CONFIRM_REQUEST, &CRequestDialog::OnBnClickedAccept)
	ON_BN_CLICKED(IDC_BUTTON_RESIST_REQUEST, &CRequestDialog::OnBnClickedReject)

END_MESSAGE_MAP()

CRequestDialog::CRequestDialog(CWnd* pParent)
	: CDialog(IDD_FRIENDREQUESTS_DIALOG, pParent)
{

}

CRequestDialog::~CRequestDialog()
{
}

void CRequestDialog::OnCancel()
{

	CMainDialog* mainDlg = dynamic_cast<CMainDialog*>(ownerDlg);
	if(mainDlg)
		AfxGetApp()->m_pMainWnd = mainDlg;
	CDialog::OnCancel();
}
void CRequestDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FRIENDREQUESTS, m_listFriendRequests);
	//DDX_Control(pDX, IDC_BUTTON_CONFIRM_REQUEST, m_btnAccept);
	//DDX_Control(pDX, IDC_BUTTON_RESIST_REQUEST, m_btnReject);
}

BOOL CRequestDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_listFriendRequests.AddString(_T("user1"));
	m_listFriendRequests.AddString(_T("user2"));

	m_bgBrush.CreateSolidBrush(COLOR_BACKGROUND);
	m_darker.CreateSolidBrush(COLOR_BACKGROUND_DARKER);

	m_btnAccept.SubclassDlgItem(IDC_BUTTON_CONFIRM_REQUEST, this);
	m_btnAccept.SetFaceColor(COLOR_BUTTON, TRUE);
	m_btnAccept.SetTextColor(RGB(255, 255, 255));

	m_btnReject.SubclassDlgItem(IDC_BUTTON_RESIST_REQUEST, this);
	m_btnReject.SetFaceColor(COLOR_DARKER_BUTTON, TRUE);
	m_btnReject.SetTextColor(RGB(255, 255, 255));

	return TRUE;
}


HBRUSH CRequestDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	int nID = pWnd->GetDlgCtrlID();

	// 전체 텍스트 색상 기본 적용
	pDC->SetTextColor(RGB(220, 221, 222));
	pDC->SetBkMode(TRANSPARENT);

	if (pWnd->GetSafeHwnd() == m_listFriendRequests.GetSafeHwnd())
	{
		pDC->SetBkColor(RGB(0, 0, 0)); // 배경 검은색
		pDC->SetTextColor(RGB(255, 255, 255)); // 글자 하얀색(가독성)
		return (HBRUSH)m_darker.GetSafeHandle();
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

void CRequestDialog::OnBnClickedAccept()
{
	int sel = m_listFriendRequests.GetCurSel();
	if (sel != LB_ERR)
	{
		CString name;
		m_listFriendRequests.GetText(sel, name);
		AfxMessageBox(name + _T(" 친구 요청 수락"));
	}
}

void CRequestDialog::OnBnClickedReject()
{
	int sel = m_listFriendRequests.GetCurSel();
	if (sel != LB_ERR)
	{
		CString name;
		m_listFriendRequests.GetText(sel, name);
		AfxMessageBox(name + _T(" 친구 요청 거절"));
	}
}
