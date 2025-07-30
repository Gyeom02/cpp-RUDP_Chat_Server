#include "pch.h"
#include "CClientApp.h"
#include "CMainDialog.h"

CClientApp ExLoginApp;

UINT WorkThread(LPVOID pParam)
{
	GUDP.GetUDPSocket(0)->UDPWork();
	return 0;
}
UINT DoJobThread(LPVOID pParam)
{
	GUDP.UDPDoJop();
	return 0;
}
UINT DoRUDPLoopThread(LPVOID pParam)
{
	int32 ackpreStart = 0;
	int32 ackStart = 1;
	int32 ackCount = 0;
	bool hasCount = false;
	NetAddress netAddr;
	while (true)
	{
		if (GPlayerManager.GetPlayers().empty())
			continue;
		for (auto p : GPlayerManager.GetPlayers())
		{
			//auto player = p.second;
			memset(&netAddr, 0, sizeof(netAddr));
			//this_thread::sleep_for(300ms);
			while (true) //AckRange 비울때까지
			{

				if (p.second->GetDeliveryManager()->WritePendingAcks(ackStart, ackCount, hasCount)) // 보낼 Ack이 쌓였다
				{
					/*if (ackpreStart == ackStart && ackpreStart > 1)
					{
						cout << "ackpreStart : " << ackpreStart << endl;
						CRASH("ackpreStart == ackStart");
					}*/
					Protocol::S_RUDPACK pkt;
					pkt.set_bhascount(hasCount);
					pkt.set_count(ackCount);
					pkt.set_start(ackStart);
					pkt.set_playerid(p.second->playerId);
					SendBufferRef sendBuffer = ServerPacketHandler::MakeUnReliableBuffer(pkt);
					p.second->Send(sendBuffer);
					//GUDP.GetUDPSocket(0)->Send(netAddr, sendBuffer);
					ackpreStart = ackStart;
					//cout << "Send RUDP ACK player->playerId : " << p.second->playerId << endl;
				}
				else
					break;
			}
			p.second->GetDeliveryManager()->ProcessTimeOutPackets();
			//PacketDeliverCondition();
		}
	}
	return 0;
}
BOOL CClientApp::InitInstance()
{
	ServerPacketHandler::Init();

	if (GUDP.UDPInit())
	{
		//AfxMessageBox(_T("UDP Init Succeed"));
		GUDP.SetIsOn(true);

		user.ownerSocket = GUDP.GetUDPSocket(0)->shared_from_this();
		user.netAddress = user.ownerSocket->GetNetAddress();

		AfxBeginThread(WorkThread, this);
		AfxBeginThread(DoJobThread, this);
		AfxBeginThread(DoRUDPLoopThread, this);
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

int CClientApp::ExitInstance()
{
	AfxThreadManager::GetInstance().StopThreads();
	m_pMainWnd->PostMessage(WM_CLOSE);
	return CWinApp::ExitInstance();
}

void AfxThreadManager::AddThread(CWinThread* thread)
{
	_threads.push_back(thread);
	return;
}

void AfxThreadManager::StopThreads()
{
	for (auto thread : _threads)
	{
		
		::TerminateThread(thread->m_hThread, 0);
	}
	_threads.clear();
}
