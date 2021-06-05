#include "stdafx.h"
#include "Global.h"
#include "SRRdtSender.h"
#include <vector>

SRRdtSender::SRRdtSender() :base(0), nextseqnum(0), isfull(false) {}

SRRdtSender::~SRRdtSender() {}


bool SRRdtSender::getWaitingState()
{
	return this->isfull;
}

bool SRRdtSender::send(const Message& msg)
{
	if (this->isfull) //窗口已满
	{
		return false;
	}

	//如果窗口未满
	Packet curPkt;
	curPkt.acknum = -1;			//制作报文
	curPkt.checksum = 0;
	curPkt.seqnum = this->nextseqnum % encodeBits;
	memcpy(curPkt.payload, msg.data, sizeof(msg.data));
	curPkt.checksum = pUtils->calculateCheckSum(curPkt);

	snd_pkt.push_back(curPkt);
	pUtils->printPacket("发送方发送报文", curPkt);
	pns->startTimer(SENDER, Configuration::TIME_OUT, curPkt.seqnum);
	pns->sendToNetworkLayer(RECEIVER, curPkt);
	this->nextseqnum++;
	this->nextseqnum %= encodeBits;
	this->isfull = (this->snd_pkt.size() < windowSize) ? false : true; //队列更新,isfull更新
	std::cout << "添加报文，发送方窗口中已发送的内容为:[ ";
	for (vector<Packet>::iterator iter = snd_pkt.begin(); iter != snd_pkt.end(); ++iter)
	{
		std::cout << iter->seqnum << ' ';
	}
	for (int i = 0; i < windowSize - (int)snd_pkt.size(); ++i)
	{
		std::cout << '#' << ' ';
	}
	std::cout << "]" << std::endl;
	return true;
}

void SRRdtSender::receive(const Packet& ackPkt)
{
	int ackCheckSum = pUtils->calculateCheckSum(ackPkt);
	int ackInWnd = -1;
	int count = 0;
	for (vector<Packet>::iterator iter = this->snd_pkt.begin(); iter != this->snd_pkt.end(); ++iter)
	{
		if ((*iter).seqnum == ackPkt.acknum)
		{
			ackInWnd = count;
			break;
		}
		count++;
	}

	if (ackCheckSum == ackPkt.checksum && ackInWnd >= 0)
	{
		pUtils->printPacket("发送方正确收到接收方发送的ACK", ackPkt);
		pns->stopTimer(SENDER, ackPkt.acknum);
		int pushable = 1;
		for (vector<int>::iterator iter = this->cf_pkt.begin(); iter != this->cf_pkt.end(); ++iter)
		{
			if ((*iter) == ackPkt.acknum)
			{
				pushable = 0;
				break;
			}
		}
		if (pushable) 
		{
			this->cf_pkt.push_back(ackPkt.acknum);
		}
		if (ackPkt.acknum == (this->snd_pkt.begin())->seqnum)
		{
			int firstUncomfiredPacket = 0; //具有最小序号的未确认分组
			for (vector<Packet>::iterator iters = this->snd_pkt.begin(); iters != this->snd_pkt.end(); ++iters)
			{
				int flag = 0;
				for (vector<int>::iterator iterc = this->cf_pkt.begin(); iterc != this->cf_pkt.end(); ++iterc)
				{
					if (iters->seqnum == (*iterc)) //若出现在已确认分组中，则firstUncomfiredPacket自增
					{
						flag = 1;
						cf_pkt.erase(iterc); //从已确认中删除最小序号的未确认分组之前的分组
						break;
					}
				}
				if (flag == 1)
					firstUncomfiredPacket++;
				else
					break;
			}
			snd_pkt.erase(this->snd_pkt.begin(), this->snd_pkt.begin() + firstUncomfiredPacket); //更新窗口内分组，删除掉最小序号的未确认分组之前的分组
			this->isfull = (this->snd_pkt.size() < windowSize) ? false : true; //队列更新,isfull更新
			this->base += firstUncomfiredPacket;
			this->base %= encodeBits;

			std::cout << "窗口滑动，发送方窗口中已发送的内容为:[ ";
			for (vector<Packet>::iterator iter = snd_pkt.begin(); iter != snd_pkt.end(); ++iter)
			{
				std::cout << iter->seqnum << ' ';
			}
			for (int i = 0; i < windowSize - (int)snd_pkt.size(); ++i)
			{
				std::cout << '#' << ' ';
			}
			std::cout << "]" << std::endl;
		}
	}
	else
	{
		pUtils->printPacket("发送方没有正确收到接收方发送的ACK", ackPkt);
	}
}

void SRRdtSender::timeoutHandler(int seqNum)
{
	for (vector<Packet>::iterator iter = this->snd_pkt.begin(); iter != this->snd_pkt.end(); ++iter)
	{
		if (iter->seqnum == seqNum)
		{
			pUtils->printPacket("发送方定时器时间到", (*iter));
			pns->stopTimer(SENDER, seqNum);
			pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
			pns->sendToNetworkLayer(RECEIVER, (*iter));
			break;
		}
	}
}
