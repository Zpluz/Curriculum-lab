#include "stdafx.h"
#include "Global.h"
#include "GBNRdtReceiver.h"

GBNRdtReceiver::GBNRdtReceiver():expectedSeqNum(0)
{
	lastAckPkt.acknum = -1; //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//忽略该字段
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
		pUtils->printPacket("接收方正确收到发送方的报文", packet);
		if (this->expectedSeqNum == packet.seqnum) //如果是预期seqnum，则上传至应用层
		{
			//将分组上传
			Message msg;
			memcpy(msg.data, packet.payload, sizeof(packet.payload));
			pns->delivertoAppLayer(RECEIVER, msg);

			//发送ACK
			this->lastAckPkt.acknum = packet.seqnum;
			this->lastAckPkt.checksum = pUtils->calculateCheckSum(this->lastAckPkt);
			pUtils->printPacket("接收方发送确认报文", this->lastAckPkt);
			pns->sendToNetworkLayer(SENDER, this->lastAckPkt);
			this->expectedSeqNum++;
			this->expectedSeqNum %= encodeBits;
		}
	}
	else
	{
		if (pktCheckSum != packet.checksum)
		{
			pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
		}
		else
		{
			pUtils->printPacket("接收方没有正确收到发送方的报文,报文失序错误", packet);
		}
		pUtils->printPacket("接收方重新发送上次的确认报文", this->lastAckPkt);
		pns->sendToNetworkLayer(SENDER, this->lastAckPkt); //重复发送上一次接收到的ACK
	}
}

