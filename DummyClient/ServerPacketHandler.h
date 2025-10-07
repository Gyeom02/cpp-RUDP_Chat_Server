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
	PKT_C_SHAREPUBLICKEY = 1022,
	PKT_S_SHAREPUBLICKEY = 1023,
	PKT_C_KEYREADY = 1024,
};

// Custom Handlers
bool Handle_INVALID(UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len);
bool Handle_S_DISCONNECT(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::S_DISCONNECT& pkt);
bool Handle_S_LOGIN(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::S_LOGIN& pkt);
bool Handle_S_MAKEACCOUNT(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::S_MAKEACCOUNT& pkt);
bool Handle_S_RUDPACK(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::S_RUDPACK& pkt);
bool Handle_S_FINDACCOUNT(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::S_FINDACCOUNT& pkt);
bool Handle_S_REQUESTFRIEND(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::S_REQUESTFRIEND& pkt);
bool Handle_S_GETFRIENDREQUEST(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::S_GETFRIENDREQUEST& pkt);
bool Handle_S_REQUESTRESPONSE(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::S_REQUESTRESPONSE& pkt);
bool Handle_S_GETFRIENDS(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::S_GETFRIENDS& pkt);
bool Handle_S_SENDMSG(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::S_SENDMSG& pkt);
bool Handle_S_GETCHATLOG(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::S_GETCHATLOG& pkt);
bool Handle_S_SHAREPUBLICKEY(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::S_SHAREPUBLICKEY& pkt);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_S_DISCONNECT] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DISCONNECT>(Handle_S_DISCONNECT, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_S_LOGIN] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LOGIN>(Handle_S_LOGIN, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_S_MAKEACCOUNT] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_MAKEACCOUNT>(Handle_S_MAKEACCOUNT, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_S_RUDPACK] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_RUDPACK>(Handle_S_RUDPACK, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_S_FINDACCOUNT] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_FINDACCOUNT>(Handle_S_FINDACCOUNT, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_S_REQUESTFRIEND] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_REQUESTFRIEND>(Handle_S_REQUESTFRIEND, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_S_GETFRIENDREQUEST] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_GETFRIENDREQUEST>(Handle_S_GETFRIENDREQUEST, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_S_REQUESTRESPONSE] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_REQUESTRESPONSE>(Handle_S_REQUESTRESPONSE, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_S_GETFRIENDS] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_GETFRIENDS>(Handle_S_GETFRIENDS, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_S_SENDMSG] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SENDMSG>(Handle_S_SENDMSG, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_S_GETCHATLOG] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_GETCHATLOG>(Handle_S_GETCHATLOG, udpSocket, clientAddr, buffer, len); };
		GPacketHandler[PKT_S_SHAREPUBLICKEY] = [](UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SHAREPUBLICKEY>(Handle_S_SHAREPUBLICKEY, udpSocket, clientAddr, buffer, len); };
	}

	static bool HandlePacket(UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE * buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](udpSocket, clientAddr, buffer, len);
	}

private:
	static SendBufferRef MakeSendBuffer(Protocol::C_DISCONNECT& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_C_DISCONNECT, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::C_LOGIN& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_C_LOGIN, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::C_MAKEACCOUNT& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_C_MAKEACCOUNT, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::C_RUDPACK& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_C_RUDPACK, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::C_FINDACCOUNT& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_C_FINDACCOUNT, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::C_REQUESTFRIEND& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_C_REQUESTFRIEND, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::C_GETFRIENDREQUEST& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_C_GETFRIENDREQUEST, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::C_REQUESTRESPONSE& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_C_REQUESTRESPONSE, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::C_GETFRIENDS& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_C_GETFRIENDS, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::C_SENDMSG& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_C_SENDMSG, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::C_GETCHATLOG& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_C_GETCHATLOG, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::C_SHAREPUBLICKEY& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_C_SHAREPUBLICKEY, breliable, class_traffic); }
	static SendBufferRef MakeSendBuffer(Protocol::C_KEYREADY& pkt,const bool& breliable= true, const uint16& class_traffic = 0) { return MakeSendBuffer(pkt, PKT_C_KEYREADY, breliable, class_traffic); }
	

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
		uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());

		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		
		vector<BYTE> input(LZ4_compressBound(dataSize));
		int compressSize = LZ4_compress_default(reinterpret_cast<const char*>(&header[1]), reinterpret_cast<char*>(input.data()), static_cast<int>(dataSize), input.size());
		
		ASSERT_CRASH(compressSize >= 0);

		if (compressSize >= dataSize)
		{
			sendBuffer->Close(packetSize);
			header->compressed = 0;
		}
		else
		{
			::memcpy(&header[1], input.data(), compressSize);
			packetSize = compressSize + sizeof(PacketHeader);
			sendBuffer->Close(packetSize);
			header->compressed = 1;
			header->decompress_size = dataSize;
			header->compress_size = compressSize;
		}
		
		header->size = packetSize;
		header->id = pktId;
		header->breliable = breliable;
		header->priority = class_traffic;
		header->retransnum = 0;

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