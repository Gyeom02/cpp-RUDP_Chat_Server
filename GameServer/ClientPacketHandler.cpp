#include "pch.h"
#include "ClientPacketHandler.h"
#include "Player.h"
#include "Room.h"
#include "PlayerManager.h"
#include "QoSCore.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "XmlParser.h"
#include "DBSynchronizer.h"
#include "GenProcedures.h"
#include "DBManager.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// ���� ������ �۾���

bool Handle_C_RUDPACK(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::C_RUDPACK& pkt)
{
	int32 bhascount = pkt.bhascount();
	int32 start = pkt.start();
	int32 count = pkt.count();
	
	PlayerRef player = GPlayerManager.GetPlayer(pkt.playerid());
	if (player == nullptr)
	{
		//cout << "Handle_C_RUDPACK PlayerID : " << pkt.playerid() << " | SequenceStart : " << pkt.start() << endl;
		return false;
	}
	player->GetDeliveyManager()->ProcessAcks(start, count, bhascount);
	//cout << "Handle_C_RUDPACK PlayerID : " << pkt.playerid() << " | SequenceStart : " << pkt.start() << endl;
	return true;
}

bool Handle_C_FINDACCOUNT(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::C_FINDACCOUNT& pkt)
{
	string getid;
	Protocol::S_FINDACCOUNT sendpkt;
	sendpkt.set_code(DBManager::Instance().FindAccount(pkt.email(), getid));
	sendpkt.set_id(getid);


	PlayerRef playerRef = MakeShared<Player>(0);
	playerRef->netAddress = clientAddr;
	playerRef->ownerSocket = udpSocket;
	SendBufferRef sendBuffer = ClientPacketHandler::MakeUnReliableBuffer(sendpkt);

	playerRef->Send(sendBuffer);

	return true;
}

bool Handle_C_DISCONNECT(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::C_DISCONNECT& pkt)
{
	cout << "GET DISCONNECT PKT By : " << pkt.id() << endl;
	int32 id = pkt.id();
	int32 roomid = pkt.roomid();
	int32 roomprimid = pkt.roomprimid();
	//PlayerManager Release
	GPlayerManager.Remove(id);
	// ID Reuse Setting
	GPlayerManager.PushID(id);
	GQoS->ErasePlayer(id);
	//Room Release
	if (roomid == -1) // �濡 ������ ����
	{

	}
	else 
	{ 
		//�濡 �� ���� (ȣ��Ʈ���� �ƴ���, �ΰ��� ������ �ƴ����� ���� �޶���(?)
		
		/*---------------------------------------------------------------------[*/
		
		
		RoomRef room = GRoom->GetRoom(roomid);
		PlayerRef player = room->GetPlayer(id);
		if (!room) // ���� �������� ����
			return true;
		if (!player) //�濡 �ش� ���̵� ���� �÷��̾ ����
		{
			cout << "Room " << roomid << " : Player ID : " << id << " Dose Not Entered" << endl;
			return true;
		}
		room->Leave(player);
			
	}
	
	return true;
}
bool Handle_INVALID(UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_C_LOGIN(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::C_LOGIN& pkt)
{
	
	string getNickname;
	int32 getPrimId;

	Protocol::S_LOGIN pktt;

	PlayerRef playerRef = nullptr;

	if (DBManager::Instance().LoginUser(pkt.id(), pkt.pw(), getPrimId, getNickname))
	{
		pktt.set_bsuccess(1);

		
		pktt.set_primid(getPrimId);
		pktt.set_nickname(getNickname);

		playerRef = MakeShared<Player>(getPrimId);
		GPlayerManager.Add(getPrimId, playerRef);
	}
	else
	{
		pktt.set_bsuccess(0);

		playerRef = MakeShared<Player>(0);
	}
	//cout << "SP Login Out PrimID : " << getPrimId << endl;
	
	
	playerRef->netAddress = clientAddr;
	playerRef->ownerSocket = udpSocket;

	auto sendbuffer = ClientPacketHandler::MakeUnReliableBuffer(pktt);
	udpSocket->Send(playerRef, sendbuffer);
	return true;
}

bool Handle_C_MAKEACCOUNT(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::C_MAKEACCOUNT& pkt)
{
	Protocol::S_MAKEACCOUNT sendpkt;
	
	if (pkt.id().size() <= 0 || pkt.id().size() >= 45)
	{
		sendpkt.set_code(3);

	}
	else if (pkt.pw().size() <= 0 || pkt.pw().size() >= 45)
	{
		sendpkt.set_code(4);
	}
	else if (pkt.nickname().size() <= 0 || pkt.nickname().size() >= 10)
	{
		sendpkt.set_code(5);
	}
	else if (pkt.email().find("@") == string::npos)
	{
		sendpkt.set_code(7);
	}
	else
	{
		sendpkt.set_code(DBManager::Instance().RegisterUser(pkt.id(), pkt.pw(), pkt.nickname(), pkt.email()));
	}
	
	PlayerRef playerRef = MakeShared<Player>(0);
	playerRef->netAddress = clientAddr;
	playerRef->ownerSocket = udpSocket;
	SendBufferRef sendBuffer = ClientPacketHandler::MakeUnReliableBuffer(sendpkt);
	playerRef->Send(sendBuffer);
	return false;
}

/*bool Handle_C_INIT(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::C_INIT& pkt)
{
	static Atomic<int32> idGenerator = 0;
	int32 reuse_id = GPlayerManager.ReuseID();
	int32 id = -1;
	if (reuse_id == -1) // ���� ������  ID�� ���� ���� �߱�
		id = ++idGenerator;
	else //����
	{
		id = reuse_id;
	}
	
	
	PlayerRef playerRef = MakeShared<Player>(id);
	//nickname �ӽ���
	playerRef->SetNickName(to_string(id));
	//nickname �ӽ���
	playerRef->netAddress = clientAddr;
	playerRef->ownerSocket = udpSocket;

	GPlayerManager.Add(id, playerRef);

	GQoS->GetShard(id)->MakeQoSPlayer(id);

	if (GPlayerManager.GetPlayer(id) == nullptr)
		cout << "NNULPTR" << endl;
	else cout << id << endl;

	Protocol::S_INIT sendpkt;

	sendpkt.set_id(id);
	sendpkt.set_nickname(playerRef->GetNickName());

	auto sendBuffer = ClientPacketHandler::MakeUnReliableBuffer(sendpkt);

	udpSocket->Send(playerRef, sendBuffer);

	return true;
}*/