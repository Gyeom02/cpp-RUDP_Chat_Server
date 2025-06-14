#include "pch.h"
#include "CClientApp.h"
#include "CMainDialog.h"

CClientApp ExLoginApp;

BOOL CClientApp::InitInstance()
{
	ServerPacketHandler::Init();

	if (GUDP.UDPInit())
	{
		//AfxMessageBox(_T("UDP Init Succeed"));
		GUDP.SetIsOn(true);
	}
	
	CLoginDialog dig;
	m_pMainWnd = &dig;
	INT_PTR nResponse = dig.DoModal();
	if (nResponse == IDOK)
	{

		//CMainDialog maindig(dig.m_strID);
		//m_pMainWnd = &maindig;
		//maindig.DoModal();
	}
	else
	{
		return FALSE;
	}
	
	return FALSE;
}
