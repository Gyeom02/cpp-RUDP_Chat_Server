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
void CRequestDialog::FreshRequestsList()
{
	for (auto& r : _requests)
	{
		m_listFriendRequests.AddString(CString(r._nickname.c_str()));
	}
}
void CRequestDialog::AddRequest(int32 id, string name)
{
	WRITE_LOCK;
	_requests.push_back(Request(id, name));
}
void CRequestDialog::AddRequests(vector<Request>& request)
{
	WRITE_LOCK;
	for (auto& v : request)
	{
		_requests.push_back(v);
	}
}
void CRequestDialog::ApplyRequestPkt(int32 index, int32 bsuccess)
{
	if (bsuccess == 1) // 성공했다
	{
		m_listFriendRequests.DeleteString(index);
	}
	else
	{
		AfxMessageBox(_T("C_REQUESTRESPONSE FAILD"));
	}
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

	user->SetCurDialog(this);

	Protocol::C_GETFRIENDREQUEST sendpkt;
	sendpkt.set_primid(to_string(user->playerId));
	SendBufferRef sendbuffer = ServerPacketHandler::MakeReliableBuffer(sendpkt, QoSCore::HIGH);
	user->Send(sendbuffer);

	FreshRequestsList();

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
		Request request = _requests[sel];
		Protocol::C_REQUESTRESPONSE sendpkt;
		sendpkt.set_primid(to_string(user->playerId));
		sendpkt.set_fprimid(to_string(request._id));
		sendpkt.set_response(1);
		sendpkt.set_listindex(sel);
		SendBufferRef sendbuffer = ServerPacketHandler::MakeReliableBuffer(sendpkt, QoSCore::HIGH);
		user->Send(sendbuffer);

		//TODO 패킷 통신 성공 여부 확인 및 UI 처리

		/*CString name;
		m_listFriendRequests.GetText(sel, name);
		AfxMessageBox(name + _T(" 친구 요청 수락"));*/
	}
}

void CRequestDialog::OnBnClickedReject()
{
	int sel = m_listFriendRequests.GetCurSel();
	if (sel != LB_ERR)
	{
		Request request = _requests[sel];
		Protocol::C_REQUESTRESPONSE sendpkt;
		sendpkt.set_primid(to_string(user->playerId));
		sendpkt.set_fprimid(to_string(request._id));
		sendpkt.set_response(0);
		sendpkt.set_listindex(sel);
		SendBufferRef sendbuffer = ServerPacketHandler::MakeReliableBuffer(sendpkt, QoSCore::HIGH);
		user->Send(sendbuffer);

	}
}
