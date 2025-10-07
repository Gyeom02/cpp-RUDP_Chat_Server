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

// 직접 컨텐츠 작업자

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
	//GPlayerManager.PushID(id);
	GQoS->ErasePlayer(id);
	//Room Release
	if (roomid == -1) // 방에 들어가있지 않음
	{

	}
	else 
	{ 
		//방에 들어가 있음 (호스트인지 아닌지, 인게임 중인지 아닌지에 따라 달라짐(?)
		
		/*---------------------------------------------------------------------[*/
		
		
		RoomRef room = GRoom->GetRoom(roomid);
		PlayerRef player = room->GetPlayer(id);
		if (!room) // 방이 존재하지 않음
			return true;
		if (!player) //방에 해당 아이디를 가진 플레이어가 없음
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
	string getfriendcode;
	Protocol::S_LOGIN pktt;

	PlayerRef playerRef = nullptr;

	if (DBManager::Instance().LoginUser(pkt.id(), pkt.pw(), getPrimId, getNickname, getfriendcode))
	{
		pktt.set_bsuccess(1);

		
		pktt.set_primid(getPrimId);
		pktt.set_nickname(getNickname);
		pktt.set_friendcode(getfriendcode);
		playerRef = MakeShared<Player>(getPrimId);
		GQoS->GetShard(getPrimId)->MakeQoSPlayer(getPrimId);
		GPlayerManager.Add(getPrimId, playerRef);
		playerRef->Getx25519().generate();
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

bool Handle_C_REQUESTFRIEND(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::C_REQUESTFRIEND& pkt)
{
	int32 result = DBManager::Instance().RequestFriend(pkt.friendcode(), pkt.primid());
	//cout << "FriendCode : " << pkt.friendcode() << endl;
	Protocol::S_REQUESTFRIEND sendpkt;
	sendpkt.set_bsuccess(result);
	SendBufferRef sendbuffer = ClientPacketHandler::MakeReliableBuffer(sendpkt, QoSCore::HIGH);

	GPlayerManager.GetPlayer(atoi(pkt.primid().c_str()))->Send(sendbuffer);
	return true;
}

bool Handle_C_GETFRIENDREQUEST(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::C_GETFRIENDREQUEST& pkt)
{
	Protocol::S_GETFRIENDREQUEST sendpkt;
	vector<int32> _ids;
	vector<string> _names;
	if (DBManager::Instance().GetRequests(pkt.primid(), _ids, _names))
	{
		for (int32 i = 0; i < _ids.size(); i++)
		{
			Protocol::Friend* f1 = sendpkt.add_requests();
			f1->set_primdid(_ids[i]);
			f1->set_nickname(_names[i]);
		}
		SendBufferRef sendbuffer = ClientPacketHandler::MakeReliableBuffer(sendpkt, QoSCore::HIGH);
		GPlayerManager.GetPlayer(atoi(pkt.primid().c_str()))->Send(sendbuffer);

		return true;
	}
	else
	{
		return false;
	}
	
	
}

bool Handle_C_REQUESTRESPONSE(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::C_REQUESTRESPONSE& pkt)
{

	Protocol::S_REQUESTRESPONSE sendpkt;
	if (DBManager::Instance().ReplyRequest(pkt.primid(), pkt.fprimid(), to_string(pkt.response())))
	{
		
		sendpkt.set_bsuccess(1);
		
	}
	else
	{
		sendpkt.set_bsuccess(0);
	
	}
	sendpkt.set_listindex(pkt.listindex());
	SendBufferRef sendbuffer = ClientPacketHandler::MakeReliableBuffer(sendpkt, QoSCore::HIGH);
	GPlayerManager.GetPlayer(atoi(pkt.primid().c_str()))->Send(sendbuffer);

	
	return true;
}

bool Handle_C_GETFRIENDS(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::C_GETFRIENDS& pkt)
{
	//cout << "Handle_C_GETFRIENDS" << endl;
	Protocol::S_GETFRIENDS sendpkt;
	vector<int32> _ids;
	vector<string> _names;
	if (DBManager::Instance().GetFriends(pkt.primid(), _ids, _names))
	{
		for (int32 i = 0; i < _ids.size(); i++)
		{
			Protocol::Friend* f1 = sendpkt.add_friends();
			f1->set_primdid(_ids[i]);
			f1->set_nickname(_names[i]);
		}
		SendBufferRef sendbuffer = ClientPacketHandler::MakeReliableBuffer(sendpkt, QoSCore::HIGH);
		GPlayerManager.GetPlayer(atoi(pkt.primid().c_str()))->Send(sendbuffer);

		return true;
	}
	else
	{
		return false;
	}
}

bool Handle_C_SENDMSG(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::C_SENDMSG& pkt)
{
	Protocol::S_SENDMSG sendpkt;
	sendpkt.set_from_id(pkt.from_id());
	sendpkt.set_to_id(pkt.to_id());
	sendpkt.set_listindex(pkt.listindex());
	SendBufferRef sendbuffer = nullptr;
	if (DBManager::Instance().SetMsg(pkt.from_id(), pkt.to_id(), pkt.msg()))
	{
		sendpkt.set_msg(pkt.msg());

		sendbuffer = ClientPacketHandler::MakeReliableBuffer(sendpkt, QoSCore::HIGH);
	
		PlayerRef receiver = GPlayerManager.GetPlayer(atoi(pkt.to_id().c_str()));
		if (receiver) // 유저가 서버에 접속해있음
			receiver->Send(sendbuffer);
	}
	else
	{
		sendpkt.set_msg("전송실패");
		sendbuffer = ClientPacketHandler::MakeReliableBuffer(sendpkt, QoSCore::HIGH);	
	}
	GPlayerManager.GetPlayer(atoi(pkt.from_id().c_str()))->Send(sendbuffer);

	return true;
}

bool Handle_C_GETCHATLOG(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::C_GETCHATLOG& pkt)
{
	Protocol::S_GETCHATLOG sendpkt;
	vector<int32> _ids;
	vector<string> _chats;
	if (DBManager::Instance().GetChats(pkt.primid(), pkt.to_id(), _ids, _chats))
	{
		for (int32 i = 0; i < _ids.size(); i++)
		{
			Protocol::ChatLog* f1 = sendpkt.add_logs();
			f1->set_primid(_ids[i]);
			f1->set_msg(_chats[i]);
		}
		SendBufferRef sendbuffer = ClientPacketHandler::MakeReliableBuffer(sendpkt, QoSCore::HIGH);
		GPlayerManager.GetPlayer(atoi(pkt.primid().c_str()))->Send(sendbuffer);

		return true;
	}
	else
	{
		return false;
	}
}

bool Handle_C_SHAREPUBLICKEY(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::C_SHAREPUBLICKEY& pkt)
{
	auto player = GPlayerManager.GetPlayer(pkt.primid());
	if (player == nullptr)
		return false;
	vector<uint8> server_pub(32);
	x25519_get_public(player->Getx25519().get(), server_pub);
	vector<uint8> ss;
	//cout << "server_pub : " << to_hex(server_pub) << endl;
	const std::string& data = pkt.publickey(); // bytes 타입은 std::string으로 매핑됨
	vector<uint8> client_pub;
	client_pub.resize(data.size());                // vector 크기 맞춤
	std::copy(data.begin(), data.end(), client_pub.begin()); // std::string -> vector 복사

	//vector<uint8> client_pub(pkt.publickey().begin(), pkt.publickey().end());
	if (!x25519_derive_shared(player->Getx25519().get(), client_pub, ss))
	{
		cout << " Handle_C_SHAREPUBLICKEY : make shared key failed" << endl;
		return false;
	}
	vector<uint8> aes_key;
	if (!make_aes256_key(ss, aes_key))
	{
		cout << " Handle_C_SHAREPUBLICKEY : make aes256 key failed" << endl;
		return false;
	}
	else 
		cout << " Handle_C_SHAREPUBLICKEY : make aes256 key : " << to_hex(aes_key) << endl;
	cout << "SIZE : " << server_pub.size() << endl;
	Protocol::S_SHAREPUBLICKEY sendpkt;
	sendpkt.set_publickey(string(server_pub.begin(), server_pub.end()));
	
	auto sendbuffer = ClientPacketHandler::MakeReliableBuffer(sendpkt, QoSCore::HIGH);
	player->Send(sendbuffer);

	player->SetAesKey(aes_key);
	return false;
}

bool Handle_C_KEYREADY(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::C_KEYREADY& pkt)
{
	if (pkt.bready() == 1)
	{
		auto player = GPlayerManager.GetPlayer(pkt.primid());
		if (player)
		{
			player->keyready = true;
		}
	}
	else
		return false;
}

/*bool Handle_C_INIT(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header, Protocol::C_INIT& pkt)
{
	static Atomic<int32> idGenerator = 0;
	int32 reuse_id = GPlayerManager.ReuseID();
	int32 id = -1;
	if (reuse_id == -1) // 재사용 가능한  ID가 없음 새로 발급
		id = ++idGenerator;
	else //재사용
	{
		id = reuse_id;
	}
	
	
	PlayerRef playerRef = MakeShared<Player>(id);
	//nickname 임시임
	playerRef->SetNickName(to_string(id));
	//nickname 임시임
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