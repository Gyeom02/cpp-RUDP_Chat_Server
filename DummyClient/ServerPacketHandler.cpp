#include "pch.h"
#include "ServerPacketHandler.h"
#include "Player.h"
#include "PlayerManager.h"
#include "CLoginDialog.h"
#include "CMainDialog.h"
#include "CSignUpDialog.h"
#include "CFindIDDialog.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// ���� ������ �۾���

bool Handle_S_RUDPACK(UDPSocketPtr udpSocket, NetAddress netAddress, PacketHeader* header, Protocol::S_RUDPACK& pkt)
{
	//cout << "Handle_S_RUDPACK" << endl;
	int32 bhascount = pkt.bhascount();
	int32 start = pkt.start();
	int32 count = pkt.count();
	/*if (pkt.playerid() == 0)
		return false;*/
	PlayerRef player = GPlayerManager.GetPlayer(header->playerId);
	//cout << "Handle_S_RUDPACK PlayerID : " << header->playerId << endl;
 	player->GetDeliveryManager()->ProcessAcks(start, count, bhascount);
	return true;
}

bool Handle_INVALID(UDPSocketPtr udpSocket, NetAddress netAddress, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}
bool Handle_S_DISCONNECT(UDPSocketPtr udpSocket, NetAddress netAddress, PacketHeader* header, Protocol::S_DISCONNECT& pkt)
{
	return false;
}

bool Handle_S_LOGIN(UDPSocketPtr udpSocket, NetAddress netAddress, PacketHeader* header, Protocol::S_LOGIN& pkt)
{
	CLoginDialog* pDlg = dynamic_cast<CLoginDialog*>(AfxGetMainWnd());
	if (pDlg)
	{
		int32 success = pkt.bsuccess();
		switch (success)
		{
		case 0: // fail
			pDlg->m_ctrlError.SetWindowTextW(_T("�α��� ���� : �ùٸ��� ���� ID �Ǵ� ��й�ȣ"));

			break;
		case 1: // success
			//user.playerId = pkt.primid();
			user.SetNickName(pkt.nickname());
			//GPlayerManager.Add(pkt.primid(), _player);

			GQoS->GetShard(pkt.primid())->MakeQoSPlayer(pkt.primid());

			//PlayerRef player = GPlayerManager.GetPlayer();

			user.playerId.store(int32(pkt.primid()));
			//cout << "Handle_S_INIT : " << _player->playerId.load() << endl;

			CMainDialog dlg;
			AfxGetApp()->m_pMainWnd = &dlg;
			pDlg->ShowWindow(SW_HIDE);
			dlg.DoModal();
			pDlg->ShowWindow(SW_SHOW);
			pDlg->EndDialog(IDOK);
		}

	}
	else
	{
		AfxMessageBox(_T("Main Dialog is not current diaog"));
	}
	//if (pkt.success() == false)
	//	return true;

	//if (pkt.players().size() == 0)
	//{
	//	// ĳ���� ����â
	//}

	//// ���� UI ��ư ������ ���� ����
	//Protocol::C_ENTER_GAME enterGamePkt;
	//enterGamePkt.set_playerindex(0); // ù��° ĳ���ͷ� ����
	//auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
	//session->Send(sendBuffer);

	return true;
}
bool Handle_S_MAKEACCOUNT(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::S_MAKEACCOUNT& pkt)
{

	CSignUpDialog* pDlg = dynamic_cast<CSignUpDialog*>(AfxGetMainWnd());
	if (pDlg == nullptr)

	{
		AfxMessageBox(_T("Handle_S_MAKEACCOUNT : ���� ���̷αװ� �ùٸ��� ����"));
		return false;
	}
	
	pDlg->SetSignUpCode(pkt.code());

	return true;
	//TODO ���̵� �ߺ��� ���̵� �Ǵ� �ٸ� ����ó�� (���� ������ Ŭ���̾�Ʈ�ʿ��� ó���ϵ��� ����)
}
bool Handle_S_FINDACCOUNT(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::S_FINDACCOUNT& pkt)
{

	CFindIdDialog* pDlg = dynamic_cast<CFindIdDialog*>(AfxGetMainWnd());
	if (pDlg == nullptr)

	{
		AfxMessageBox(_T("Handle_S_FINDACCOUNT : ���� ���̷αװ� �ùٸ��� ����"));
		return false;
	}

	pDlg->SetFindCode(pkt.code());
	pDlg->SetFindID(pkt.id());
	return true;
	//TODO ���̵� �ߺ��� ���̵� �Ǵ� �ٸ� ����ó�� (���� ������ Ŭ���̾�Ʈ�ʿ��� ó���ϵ��� ����)
}


/*
bool Handle_S_INIT(UDPSocketPtr udpSocket, NetAddress netAddress, PacketHeader* header, Protocol::S_INIT& pkt)
{
	PlayerRef _player = MakeShared<Player>(pkt.id());
	_player->ownerSocket = GUDP.GetUDPSocket(0)->shared_from_this();
	_player->netAddress = _player->ownerSocket->GetNetAddress();
	GPlayerManager.Add(pkt.id(), _player);

	GQoS->GetShard(pkt.id())->MakeQoSPlayer(pkt.id());

	//PlayerRef player = GPlayerManager.GetPlayer();

	_player->playerId.store(int32(pkt.id()));
	cout << "Handle_S_INIT : " << _player->playerId.load() << endl;
	return true;
}
*/