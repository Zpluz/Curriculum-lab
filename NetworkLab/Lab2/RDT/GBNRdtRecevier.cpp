#include "stdafx.h"
#include "Global.h"
#include "GBNRdtReceiver.h"

GBNRdtReceiver::GBNRdtReceiver():expectedSeqNum(0)
{
	lastAckPkt.acknum = -1; //��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ-1��ʹ�õ���һ�����ܵ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//���Ը��ֶ�
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) 
	{
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}

GBNRdtReceiver::~GBNRdtReceiver(){}

void GBNRdtReceiver::receive(const Packet& packet)
{
	int pktCheckSum = pUtils->calculateCheckSum(packet);
	if (pktCheckSum == packet.checksum && packet.seqnum == this->expectedSeqNum)
	{
		pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���", packet);
		if (this->expectedSeqNum == packet.seqnum) //�����Ԥ��seqnum�����ϴ���Ӧ�ò�
		{
			//�������ϴ�
			Message msg;
			memcpy(msg.data, packet.payload, sizeof(packet.payload));
			pns->delivertoAppLayer(RECEIVER, msg);

			//����ACK
			this->lastAckPkt.acknum = packet.seqnum;
			this->lastAckPkt.checksum = pUtils->calculateCheckSum(this->lastAckPkt);
			pUtils->printPacket("���շ�����ȷ�ϱ���", this->lastAckPkt);
			pns->sendToNetworkLayer(SENDER, this->lastAckPkt);
			this->expectedSeqNum++;
			this->expectedSeqNum %= encodeBits;
		}
	}
	else
	{
		if (pktCheckSum != packet.checksum)
		{
			pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
		}
		else
		{
			pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����ʧ�����", packet);
		}
		pUtils->printPacket("���շ����·����ϴε�ȷ�ϱ���", this->lastAckPkt);
		pns->sendToNetworkLayer(SENDER, this->lastAckPkt); //�ظ�������һ�ν��յ���ACK
	}
}

