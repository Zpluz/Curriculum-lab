#include "stdafx.h"
#include "Global.h"
#include "TCPRdtReceiver.h"

#include <vector>

TCPRdtReceiver::TCPRdtReceiver() :expectedSeqNum(0)
{
	lastAckPkt.acknum = -1; //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//忽略该字段
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++)
	{
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
	for (int i = 0; i < windowSize; ++i)
	{
		expSeqnum.push_back(i % encodeBits); //预期窗口初始化0,1,2,3
	}
}

TCPRdtReceiver::~TCPRdtReceiver() {}

void TCPRdtReceiver::receive(const Packet& packet)
{
	int pktCheckSum = pUtils->calculateCheckSum(packet);
	int inExpWnd = 0; //是否在预期窗口内
	int inRcvWnd = -1; //是否已被缓存
	if (pktCheckSum == packet.checksum)
	{
		for (vector<int>::iterator itern = expSeqnum.begin(); itern != expSeqnum.end(); ++itern)
		{
			if (packet.seqnum == (*itern)) //说明该包在预期窗口内
			{
				inExpWnd = 1;
				for (vector<Packet>::iterator iterr = rcv_pkt.begin(); iterr != rcv_pkt.end(); ++iterr)
				{
					if ((*iterr) == packet)
					{
						inRcvWnd = 1;
						break;
					}
				}
				if (inExpWnd && inRcvWnd == -1)  //说明包在预期窗口内且没有被缓存
				{
					rcv_pkt.push_back(packet); //缓存该包
				}
				break;
			}
		}

		std::cout << "接收包" << packet.seqnum << "，预期接收窗口为:[ ";
		for (vector<int>::iterator iter = expSeqnum.begin(); iter != expSeqnum.end(); ++iter)
		{
			std::cout << *iter << ' ';
		}
		std::cout << "]，接收方滑动窗口中的内容为:[ ";
		for (vector<Packet>::iterator iter = rcv_pkt.begin(); iter != rcv_pkt.end(); ++iter)
		{
			std::cout << iter->seqnum << ' ';
		}
		for (int i = 0; i < windowSize - (int)rcv_pkt.size(); ++i)
		{
			std::cout << '#' << ' ';
		}
		std::cout << "]" << std::endl;

		if (inExpWnd == 1) //若在窗口内
		{
			if (packet.seqnum == (*expSeqnum.begin())) //若接收到的包为预期窗口内的base,则将其后连续的已缓存包上传
			{
				int expNextBase = 0; //预期的下一个Base，用于更新expSeqNum
				for (vector<int>::iterator itern = expSeqnum.begin(); itern != expSeqnum.end(); ++itern)
				{
					int continuous = 0;
					for (vector<Packet>::iterator iterr = rcv_pkt.begin(); iterr != rcv_pkt.end(); ++iterr)
					{
						if (iterr->seqnum == (*itern))
						{
							this->lastAckPkt.acknum = iterr->seqnum;
							this->lastAckPkt.checksum = pUtils->calculateCheckSum(this->lastAckPkt);
							pUtils->printPacket("接收方发送确认报文", this->lastAckPkt);
							pns->sendToNetworkLayer(SENDER, this->lastAckPkt);

							continuous = 1;
							Message msg;
							memcpy(msg.data, iterr->payload, sizeof(iterr->payload));
							pns->delivertoAppLayer(RECEIVER, msg);
							rcv_pkt.erase(iterr); //已上传的包从rcv_pkt中删掉
							break;
						}
					}
					if (!continuous) //此处连续发包，不连续中断
						break;
					expNextBase++;
				}

				//此处更新expSeqNum
				int lastSeq = expSeqnum.back();
				expSeqnum.erase(expSeqnum.begin(), expSeqnum.begin() + expNextBase);
				while (expSeqnum.size() < windowSize)
				{
					expSeqnum.push_back((lastSeq + 1) % encodeBits); //预期窗口从expNextBase开始更新
					lastSeq = expSeqnum.back();
				}

				std::cout << "窗口滑动，预期接收窗口的内容为:[ ";
				for (vector<int>::iterator iter = expSeqnum.begin(); iter != expSeqnum.end(); ++iter)
				{
					std::cout << *iter << ' ';
				}
				std::cout << "]，接收方滑动窗口中的内容为:[ ";
				for (vector<Packet>::iterator iter = rcv_pkt.begin(); iter != rcv_pkt.end(); ++iter)
				{
					std::cout << iter->seqnum << ' ';
				}
				for (int i = 0; i < windowSize - (int)rcv_pkt.size(); ++i)
				{
					std::cout << '#' << ' ';
				}
				std::cout << "]" << std::endl;
			}
			else
			{
				pUtils->printPacket("接收方没有正确收到发送方的报文,报文失序但已被缓存", packet);
				pUtils->printPacket("接收方重新发送上次的确认报文", this->lastAckPkt);
				pns->sendToNetworkLayer(SENDER, this->lastAckPkt); //重复发送上一次接收到的ACK
			}
		}
		else
		{
			//不在预期窗口内，也要返回ACK
			//有可能返回ACK错误导致重发，而这边上传给应用层导致窗口前滑使得重发的包号不在窗口中，发送方接收不到ACK不停止计时器就会不断超时重传，所以需要再发ACK来使发送方窗口向前移动
			Packet ackPkt;				
			ackPkt.seqnum = -1;
			ackPkt.checksum = 0;
			ackPkt.acknum = packet.seqnum;
			for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++)
			{
				ackPkt.payload[i] = '.';
			}
			ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
			pUtils->printPacket("接收方发送确认报文", ackPkt);
			pns->sendToNetworkLayer(SENDER, ackPkt);
		}
		
	}
	else
	{
		pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
	}
}

