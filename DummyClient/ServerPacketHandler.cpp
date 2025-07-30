#include "pch.h"
#include "ServerPacketHandler.h"
#include "Player.h"
#include "PlayerManager.h"
#include "CLoginDialog.h"
#include "CMainDialog.h"
#include "CSignUpDialog.h"
#include "CFindIDDialog.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// 직접 컨텐츠 작업자

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
			pDlg->m_ctrlError.SetWindowTextW(_T("로그인 실패 : 올바르지 않은 ID 또는 비밀번호"));

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
	//	// 캐릭터 생성창
	//}

	//// 입장 UI 버튼 눌러서 게임 입장
	//Protocol::C_ENTER_GAME enterGamePkt;
	//enterGamePkt.set_playerindex(0); // 첫번째 캐릭터로 입장
	//auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
	//session->Send(sendBuffer);

	return true;
}
bool Handle_S_MAKEACCOUNT(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::S_MAKEACCOUNT& pkt)
{

	CSignUpDialog* pDlg = dynamic_cast<CSignUpDialog*>(AfxGetMainWnd());
	if (pDlg == nullptr)

	{
		AfxMessageBox(_T("Handle_S_MAKEACCOUNT : 현재 다이로그가 올바르지 않음"));
		return false;
	}
	
	pDlg->SetSignUpCode(pkt.code());

	return true;
	//TODO 아이디 중복된 아이디 또는 다른 에러처리 (길이 문제는 클라이언트쪽에서 처리하도록 변경)
}
bool Handle_S_FINDACCOUNT(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::S_FINDACCOUNT& pkt)
{

	CFindIdDialog* pDlg = dynamic_cast<CFindIdDialog*>(AfxGetMainWnd());
	if (pDlg == nullptr)

	{
		AfxMessageBox(_T("Handle_S_FINDACCOUNT : 현재 다이로그가 올바르지 않음"));
		return false;
	}

	pDlg->SetFindCode(pkt.code());
	pDlg->SetFindID(pkt.id());
	return true;
	//TODO 아이디 중복된 아이디 또는 다른 에러처리 (길이 문제는 클라이언트쪽에서 처리하도록 변경)
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