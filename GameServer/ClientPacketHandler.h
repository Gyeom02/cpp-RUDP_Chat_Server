#pragma once
#include "Protocol.pb.h"
#include "UDP.h"
#include "NetAddress.h"
//#include "DeliveryNotificationManager.h"
#include "Player.h"
#include "QoSCore.h"

using PacketHandlerFunc = std::function<bool(UDPSocketPtr, NetAddress, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_DISCONNECT = 1000,
	PKT_S_DISCONNECT = 1001,
	PKT_C_LOGIN = 1002,
	PKT_S_LOGIN = 1003,
	PKT_C_MAKEACCOUNT = 1004,
	PKT_S_MAKEACCOUNT = 1005,
	PKT_C_RUDPACK = 1006,
	PKT_S_RUDPACK = 1007,
	PKT_C_FINDACCOUNT = 1008,
	PKT_S_FINDACCOUNT = 1009,
	PKT_C_REQUESTFRIEND = 1010,
	PKT_S_REQUESTFRIEND = 1011,
	PKT_C_GETFRIENDREQUEST = 1012,
	PKT_S_GETFRIENDREQUEST = 1013,
	PKT_C_REQUESTRESPONSE = 1014,
	PKT_S_REQUESTRESPONSE = 1015,
	PKT_C_GETFRIENDS = 1016,
	PKT_S_GETFRIENDS = 1017,
	PKT_C_SENDMSG = 1018,
	PKT_S_SENDMSG = 1019,
	PKT_C_GETCHATLOG = 1020,
	PKT_S_GETCHATLOG = 1021,
};

// Custom Handlers
bool Handle_INVALID(UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len);
bool Handle_C_DISCONNECT(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::C_DISCONNECT& pkt);
bool Handle_C_LOGIN(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::C_LOGIN& pkt);
bool Handle_C_MAKEACCOUNT(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::C_MAKEACCOUNT& pkt);
bool Handle_C_RUDPACK(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::C_RUDPACK& pkt);
bool Handle_C_FINDACCOUNT(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::C_FINDACCOUNT& pkt);
bool Handle_C_REQUESTFRIEND(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::C_REQUESTFRIEND& pkt);
bool Handle_C_GETFRIENDREQUEST(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::C_GETFRIENDREQUEST& pkt);
bool Handle_C_REQUESTRESPONSE(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::C_REQUESTRESPONSE& pkt);
bool Handle_C_GETFRIENDS(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::C_GETFRIENDS& pkt);
bool Handle_C_SENDMSG(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::C_SENDMSG& pkt);
bool Handle_C_GETCHATLOG(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::C_GETCHATLOG& pkt);

class ClientPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_C_DISCONNECT] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_DISCONNECT>(Handle_C_DISCONNECT, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_C_LOGIN] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_LOGIN>(Handle_C_LOGIN, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_C_MAKEACCOUNT] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_MAKEACCOUNT>(Handle_C_MAKEACCOUNT, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_C_RUDPACK] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_RUDPACK>(Handle_C_RUDPACK, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_C_FINDACCOUNT] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_FINDACCOUNT>(Handle_C_FINDACCOUNT, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_C_REQUESTFRIEND] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_REQUESTFRIEND>(Handle_C_REQUESTFRIEND, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_C_GETFRIENDREQUEST] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_GETFRIENDREQUEST>(Handle_C_GETFRIENDREQUEST, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_C_REQUESTRESPONSE] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_REQUESTRESPONSE>(Handle_C_REQUESTRESPONSE, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_C_GETFRIENDS] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_GETFRIENDS>(Handle_C_GETFRIENDS, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_C_SENDMSG] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_SENDMSG>(Handle_C_SENDMSG, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_C_GETCHATLOG] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_GETCHATLOG>(Handle_C_GETCHATLOG, udpSocket, clientAddr, buffer, len); };
	}

	static bool HandlePacket(UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE * buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](udpSocket, clientAddr, buffer, len);
	}

private:
	static SendBufferRef MakeSendBuffer(Protocol::S_DISCONNECT& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_S_DISCONNECT, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::S_LOGIN& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_S_LOGIN, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::S_MAKEACCOUNT& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_S_MAKEACCOUNT, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::S_RUDPACK& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_S_RUDPACK, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::S_FINDACCOUNT& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_S_FINDACCOUNT, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::S_REQUESTFRIEND& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_S_REQUESTFRIEND, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::S_GETFRIENDREQUEST& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_S_GETFRIENDREQUEST, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::S_REQUESTRESPONSE& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_S_REQUESTRESPONSE, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::S_GETFRIENDS& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_S_GETFRIENDS, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::S_SENDMSG& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_S_SENDMSG, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::S_GETCHATLOG& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_S_GETCHATLOG, breliable, class_traffic); }
	

	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE * buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(udpSocket, clientAddr, header, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId, const bool& breliable, const uint16& class_traffic) 
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		header->breliable = breliable;
		header->priority = class_traffic;
		header->retransnum = 0;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}

public:
	template<typename PKT>
	static SendBufferRef MakeReliableBuffer(PKT pkt, uint16 priority)
	{
		if constexpr (requires { MakeSendBuffer(pkt, true, priority); }) {
			_ASSERT(priority < QoSCore::MAX); 
			return MakeSendBuffer(pkt, true, priority); 
		}
		
		return nullptr;
	}
	template<typename PKT>
	static SendBufferRef MakeUnReliableBuffer(PKT pkt)
	{
		if constexpr (requires { MakeSendBuffer(pkt, false, QoSCore::FPC); }) {
			return MakeSendBuffer(pkt, false, QoSCore::FPC);
			
		}
		return nullptr;
	}
};