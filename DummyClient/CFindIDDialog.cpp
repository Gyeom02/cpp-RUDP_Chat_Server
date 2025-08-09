#include "pch.h"
#include "CFindIDDialog.h"
#include "CLoginDialog.h"

BEGIN_MESSAGE_MAP(CFindIdDialog, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_SEARCH_ID, &CFindIdDialog::OnBnClickedFind)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH_BACK, &CFindIdDialog::OnBnClickedExit)

END_MESSAGE_MAP()

CFindIdDialog::CFindIdDialog(CWnd* pParent)
	: CDialog(IDD_FINDID_DIALOG, pParent)
{
}

CFindIdDialog::~CFindIdDialog()
{
}

BOOL CFindIdDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	user->SetCurDialog(this);

	m_bgBrush.CreateSolidBrush(COLOR_BACKGROUND);

	m_findBtn.SubclassDlgItem(IDC_BTN_SEARCH_ID, this);
	m_findBtn.SetFaceColor(COLOR_BUTTON, TRUE);
	m_findBtn.SetTextColor(RGB(255, 255, 255));
	return TRUE;
}

void CFindIdDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_EMAIL, m_strEmail);
	DDX_Control(pDX, IDC_FINDID_RESULT, m_staticResult);
//	DDX_Control(pDX, IDC_BTN_SEARCH_ID, m_findBtn);
}


HBRUSH CFindIdDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	int nID = pWnd->GetDlgCtrlID();

	// ��ü �ؽ�Ʈ ���� �⺻ ����
	pDC->SetTextColor(RGB(220, 221, 222));
	pDC->SetBkMode(TRANSPARENT);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		if (pWnd->GetDlgCtrlID() == IDC_FINDID_RESULT)  // ��� Static ID
		{
			pDC->SetTextColor(m_resultColor);
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

void CFindIdDialog::OnBnClickedFind()
{
	UpdateData(TRUE);

	if (m_strEmail.IsEmpty())
	{
		AfxMessageBox(_T("�̸����� �Է����ּ���"));
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
		CT2CA convertedString(m_strEmail);
		std::string email = std::string(convertedString);
		
		Protocol::C_FINDACCOUNT sendpkt;
		sendpkt.set_email(email);

		SendBufferRef snedBuffer = ServerPacketHandler::MakeUnReliableBuffer(sendpkt);
		user->BlockSend(snedBuffer);
		user->BlockRecv();

		int32 code = GetFindCode();
		CLoginDialog dlg;
		CString getemial;
		switch (code)
		{
		case -1:
			AfxMessageBox(_T("Unknown DB Failiure"));
			break;
		case 0:
			m_staticResult.SetWindowTextW(_T("������ ã�� ���߽��ϴ�"));
			break;
		case 1: // ����
			getemial = GetFindID().c_str();
	
			m_staticResult.SetWindowTextW(getemial);
			break;
		
		default:
			break;
		}
		SetResultColor(code);
	}
}

void CFindIdDialog::OnBnClickedExit()
{
	if (AfxMessageBox(_T("���ư��ðڽ��ϱ�?"), MB_YESNO) == IDYES)
	{
		CLoginDialog dlg;
		AfxGetApp()->m_pMainWnd = &dlg;
		this->ShowWindow(SW_HIDE);
		dlg.DoModal();
		this->ShowWindow(SW_SHOW);
		EndDialog(IDOK);
	}
}

void CFindIdDialog::SetResultColor(int32 code)
{
	if (code == 0) // Set Green
	{
		m_resultColor = RGB(255, 0, 0);
	}
	else // SetRed
	{
		m_resultColor = COLOR_BUTTON;
	}
	Invalidate();
}


BOOL CFindIdDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		OnBnClickedFind();
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

