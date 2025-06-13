#include "pch.h"
#include "InFlightPacket.h"

void InFlightPacket::HandleDeliveryFailure(DeliveryManagerRef deliveryManager)
{
	SendBufferRef sendBuffer = GetTransmissionData();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
	header->retransnum++;
	if (header->retransnum > 3) // 3���� ������ ��ȸ�� �پ�
	{
		header->retransnum = 0;
		//Pass
	}
	else // ���� ������ �� �� ����
	{
	//	cout << "ReSend" << endl;
		deliveryManager->WriteSeqeuenceNumber(_socket, GetNetAddr(), sendBuffer);
		Send();
	}
}

void InFlightPacket::HandleDeliverySuccess(DeliveryManagerRef deliveryManager)
{
	SendBufferRef sendBuffer = GetTransmissionData();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
	if (header->retransnum >= 1) // ��������Ŷ �����ߴٴ� ���̴�
	{
		//cout << "header->retransnum : " << header->retransnum << endl;
		//int32 n = GDeliveryManager->GetSuccessReSendPacketNum();
		//cout << "header->retransnum : " << header->retransnum << endl;

		deliveryManager->mSuccessReSendPacketNum += header->retransnum; // 1�� ó���� ���� ���� �ǹ�
	}
	header->retransnum = 0;
}

int32 InFlightPacket::Send()
{

	SOCKADDR_IN netaddr = _netAddr.GetSockAddr();
	//cout << netAddr.GetPort()<< endl;
	//wcout << netAddr.GetIpAddress() << endl;
	int32 addrLen = sizeof(netaddr);
	while (true)
	{
		if (::sendto(_socket, reinterpret_cast<const char*>(_sendBuffer->Buffer()), _sendBuffer->WriteSize(), 0, reinterpret_cast<SOCKADDR*>(&netaddr), addrLen) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;
			break;
		}
		else break;
	}
	return _sendBuffer->WriteSize();
	


}
