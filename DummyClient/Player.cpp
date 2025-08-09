#include "pch.h"
#include "Player.h"

PlayerRef user = make_shared<Player>(0);
Player::Player(int32 id) 
	: playerId(id), roomId(-1), roomprimid(-1), teamNum(-1)
{
	deliveryManager = MakeShared<DeliveryNotificationManager>();
}
void Player::Send(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	ownerSocket->Send(static_pointer_cast<Player>(shared_from_this()), sendBuffer);
}
void Player::PriortySend(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	ownerSocket->PriortySend(static_pointer_cast<Player>(shared_from_this()), sendBuffer);
}
void Player::SetImport(Position _pos, Rotation _rot, Velocity _vel)
{
	pos = _pos;
	rot = _rot;
	vel = _vel;
}

void Player::SetNickName(string nickname)
{
	WRITE_LOCK;
	_nickname = nickname;
}

string Player::GetNickName()
{
	READ_LOCK;
	return _nickname;
}

void Player::SetFriendCode(string nickname)
{
	WRITE_LOCK;
	_friendcode = nickname;
}

string Player::GetFriendCode()
{
	READ_LOCK;
	return _friendcode;
}


int32 Player::BlockSend(SendBufferRef sendBuffer)
{
	SOCKADDR_IN netaddr = GUDP.GetUDPSocket(0)->GetNetAddress().GetSockAddr();
	UDPSocketPtr socketPtr = GUDP.GetUDPSocket(0);
	//cout << netAddr.GetPort()<< endl;
	//wcout << netAddr.GetIpAddress() << endl;
	int32 addrLen = sizeof(netaddr);
	while (true)
	{
		if (::sendto(socketPtr->GetSocket(), reinterpret_cast<const char*>(sendBuffer->Buffer()), sendBuffer->WriteSize(), 0, reinterpret_cast<SOCKADDR*>(&netaddr), addrLen) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;
			cout << "Send Faild : " << ::WSAGetLastError() << endl;
			break;
		}
		else
		{
			cout << "Send Success : " << sendBuffer->WriteSize() << endl;
			break;
		}
	}
	return sendBuffer->WriteSize();
}

int32 Player::BlockRecv()
{
	RecvBuffer recvBuffer(65536);
	PacketHeader* header = reinterpret_cast<PacketHeader*>(recvBuffer.WritePos());


	SOCKADDR_IN netaddr;
	memset(&netaddr, 0, sizeof(SOCKADDR_IN));
	//cout << netAddr.GetPort()<< endl;
	//wcout << netAddr.GetIpAddress() << endl;
	int32 addrLen = sizeof(netaddr);
	int32 Len;
	while (true)
	{
		Len = ::recvfrom(ownerSocket->GetSocket(), reinterpret_cast<char*>(recvBuffer.WritePos()), recvBuffer.FreeSize(), 0, reinterpret_cast<SOCKADDR*>(&netaddr), &addrLen);
		if (Len == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}

			cout << "Recv Faild : " << ::WSAGetLastError() << endl;
			break;
		}
		else {
			cout << "Recv Success : " << Len << endl;
			break;
		}
	}
	//cout << "Recved" << endl;
	ServerPacketHandler::HandlePacket(ownerSocket, ownerSocket->GetNetAddress(), recvBuffer.WritePos(), Len);
	return recvBuffer.DataSize();
}
