#include "pch.h"
#include "CClientApp.h"
#include "CMainDialog.h"

CClientApp ExLoginApp;
UINT ReplyRecved(LPVOID pParam)
{
	int32 start = 0;
	int32 count = 0;
	bool bhascount;
	while (true)
	{
		//this_thread::sleep_for(100ms);
		if (user != nullptr && user->playerId != 0)
		{
			//this_thread::sleep_for(100ms);
			if (user->GetDeliveryManager()->WritePendingAcks(start, count, bhascount)) // ���� Ack�� �׿���
			{
				Protocol::C_RUDPACK pkt;
				pkt.set_bhascount(bhascount);
				pkt.set_count(count);
				pkt.set_start(start);
				pkt.set_playerid(user->playerId);
				SendBufferRef sendBufferR = ServerPacketHandler::MakeUnReliableBuffer(pkt);
				user->Send(sendBufferR);
				//cout << "Send RUDP ACK" << endl;
			}
		}
		user->GetDeliveryManager()->ProcessTimeOutPackets();
		//PacketLost();
		//PacketDeliverCondition(player);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
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

BOOL CClientApp::InitInstance()
{
	ServerPacketHandler::Init();

	if (GUDP.UDPInit())
	{
		//AfxMessageBox(_T("UDP Init Succeed"));
		GUDP.SetIsOn(true);

		user->ownerSocket = GUDP.GetUDPSocket(0)->shared_from_this();
		user->netAddress = user->ownerSocket->GetNetAddress();

		AfxBeginThread(WorkThread, this);
		AfxBeginThread(DoJobThread, this);
		AfxBeginThread(ReplyRecved, this);
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
void CClientApp::SendExitPkt()
{
	if (user)
	{
		if (user->playerId > 0)
		{
			Protocol::C_DISCONNECT pkt;
			pkt.set_id(user->playerId);
			pkt.set_roomid(user->roomId);
			pkt.set_roomprimid(user->roomprimid);
			SendBufferRef sendBuffer = ServerPacketHandler::MakeReliableBuffer(pkt, QoSCore::HIGH);

			user->Send(sendBuffer);
		}
	}
}

int CClientApp::ExitInstance()
{
	SendExitPkt();
	AfxThreadManager::GetInstance().StopThreads();
	
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

