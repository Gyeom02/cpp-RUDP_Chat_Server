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
};

// Custom Handlers
bool Handle_INVALID(UDPSocketPtr udpSocket, NetAddress clientAddr, BYTE* buffer, int32 len);
bool Handle_C_DISCONNECT(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::C_DISCONNECT& pkt);
bool Handle_C_LOGIN(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::C_LOGIN& pkt);
bool Handle_C_MAKEACCOUNT(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::C_MAKEACCOUNT& pkt);
bool Handle_C_RUDPACK(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::C_RUDPACK& pkt);
bool Handle_C_FINDACCOUNT(UDPSocketPtr udpSocket, NetAddress clientAddr, PacketHeader* header,  Protocol::C_FINDACCOUNT& pkt);

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