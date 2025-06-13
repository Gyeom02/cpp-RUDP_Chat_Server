#include "pch.h"
#include "DeliveryNotificationManager.h"

DeliveryNotificationManager::DeliveryNotificationManager()
{
}

DeliveryNotificationManager::~DeliveryNotificationManager()
{
}

InFlightPacketPtr DeliveryNotificationManager::WriteSeqeuenceNumber(SOCKET object, NetAddress netAddr, SendBufferRef sendBuffer)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
	PacketSequenceNumber sequenceNumber = mNextOutgoingSequenceNumber++;
	header->sn = sequenceNumber.GetSN(); // ��Ŷ ����� SequenceNumber ����

	++mDispatchedPacketCount;

	WRITE_LOCK;

	mInFlightPackets.emplace_back(MakeShared<InFlightPacket>(object, netAddr, sequenceNumber, sendBuffer));
	return mInFlightPackets.back();
}

void DeliveryNotificationManager::ProcessAcks(int32 start, int32 count, bool hasCount)
{
	AckRange ackRange;
	ackRange.AckRead(start, count);

	PacketSequenceNumber nextAckdSequenceNumber = ackRange.GetStart();
	int32 onePastAckdSequenceNumber = nextAckdSequenceNumber.GetSN() + ackRange.GetCount();

	WRITE_LOCK;

	while (nextAckdSequenceNumber.GetSN() < onePastAckdSequenceNumber && !mInFlightPackets.empty())
	{
		const auto& nextInFlightPacket = mInFlightPackets.front();
		//��Ŷ�� ������ ��ȣ�� Ȯ�������� ������ ��ȣ���� ������ Ȯ�������� ���� ���� ���̹Ƿ� �Ƹ� �����Ǿ��� ����
		PacketSequenceNumber nextInFlightPacketSequenceNumber = nextInFlightPacket->GetSequenceNumber();

		if (nextInFlightPacketSequenceNumber.GetSN() < nextAckdSequenceNumber.GetSN())
		{
			//�纻�� �������, ��Ͽ��� �ϴ� ������
			//�ڵ鸵 ���� ����ִ� ��Ŷ�� �������� ã�ƺ� �� �� ��Ŷ�� �������� �ȵǱ� �����̴�.
			auto copyOfInFlightPacket = nextInFlightPacket;
			mInFlightPackets.pop_front();
			HandlePacketDeliveryFailure(copyOfInFlightPacket);
			mSequenceNotMatchedCount++;
			//cout << nextInFlightPacketSequenceNumber.GetSN() << " < " << nextAckdSequenceNumber.GetSN() << endl;
		}
		else if (nextInFlightPacketSequenceNumber == nextAckdSequenceNumber)
		{
		//	cout << "DeliverySuccess" << endl;
			HandlePacketDeliverySuccess(nextInFlightPacket);
			mInFlightPackets.pop_front();
			++nextAckdSequenceNumber;
		}

		else if (nextInFlightPacketSequenceNumber > nextAckdSequenceNumber)
		{
			//�Ϻ� ������ � ���������� ���ŵǾ���(�ð� �ʰ� ���ɼ�)
			//�������� ����Ͽ� �˻���
		//	cout << "nextInFlightPacketSequenceNumber > nextAckdSequenceNumber" << endl;
			nextAckdSequenceNumber = nextInFlightPacketSequenceNumber;
		}
	}
}

void DeliveryNotificationManager::HandlePacketDeliveryFailure(const InFlightPacketPtr& inFlightPacket)
{
	++mDroppedPacketCount;
	inFlightPacket->HandleDeliveryFailure(shared_from_this());
}

void DeliveryNotificationManager::HandlePacketDeliverySuccess(const InFlightPacketPtr& inFlightPacket)
{
	++mDeliveredPacketCount;
	//cout << "DeliveredCount : " << mDeliveredPacketCount << endl;
	inFlightPacket->HandleDeliverySuccess(shared_from_this());
}

bool DeliveryNotificationManager::ProcessSequenceNumber(PacketSequenceNumber SN)
{
	if (SN.GetSN() >= mNextExpectedSequenceNumber) //�����ϴ� ���� ��Ŷ �������ѹ��� ����
	{
		mNextExpectedSequenceNumber = SN.GetSN() + 1;
		AddPendingAck(SN);
		return true;
	}

	else if (SN.GetSN() < mNextExpectedSequenceNumber) //��ٸ��� �־��� ���� ��Ŷ ������ �ѹ��� �ƴ� ������ �ѱ�
	{
		return false;
	}

	return false;

}

void DeliveryNotificationManager::AddPendingAck(PacketSequenceNumber SN)
{
	WRITE_LOCK;
	if (mPendingAcks.size() == 0 || !mPendingAcks.back().ExtendIfShould(SN))
	{
		mPendingAcks.emplace_back(SN.GetSN());
	}
}

bool DeliveryNotificationManager::WritePendingAcks(OUT int32& start, OUT int32& count, OUT bool& hasCount)
{
	WRITE_LOCK;
	bool hasAcks = (mPendingAcks.size() > 0);
	if (hasAcks)
	{
		mPendingAcks.front().AckWrite(start, count, hasCount);
		mPendingAcks.pop_front();
		return true;
	}
	return false;
}

void DeliveryNotificationManager::ProcessTimeOutPackets()
{
	WRITE_LOCK;
	uint64 now = GetTickCount64();
	while (!mInFlightPackets.empty())
	{
		const auto& nextInFlightPacket = mInFlightPackets.front();
		if (now - nextInFlightPacket->GetTimeDispactched() > TIMEOUT)
		{
			mTimeOutCount++;
			PacketHeader* header = reinterpret_cast<PacketHeader*>(nextInFlightPacket->GetTransmissionData()->Buffer());
		//	cout << " TimeOutPacket ID : " << header->id << " | SN : " << header->sn << endl;
			HandlePacketDeliveryFailure(nextInFlightPacket);
			mInFlightPackets.pop_front();
		}
		else
			return; //���� ���� ��Ŷ���ʹ� �ʰ��� �ƴ�(�ð�������� �־����ֱ� �����̴�)
	}
}

bool AckRange::ExtendIfShould(PacketSequenceNumber SN)
{
	if (SN.GetSN() == mStart + mCount)
	{
		++mCount;
		return true;
	}
	else
	{
		return false;
	}
}

void AckRange::AckWrite(OUT int32& start, OUT int32& count, OUT bool& hasCount)
{
	start = mStart;
	hasCount = mCount > 1;
	if (hasCount)
	{
		int32 countMinusOne = mCount - 1; // ���� �� ���� ������ ��Ŷ ��
		uint8 countToAck = countMinusOne > 255 ? 255 : static_cast<uint8>(countMinusOne);
		count = static_cast<int32>(countToAck);
	}
	//netAddr = netAddress;
}

void AckRange::AckRead(int32 start, int32 count)
{
	mStart = start;

	bool hasCount = count > 0;
	if (hasCount)
	{
		mCount = count + 1;
	}
	else mCount = 1; // default

	//netAddress = netAddr;
}
