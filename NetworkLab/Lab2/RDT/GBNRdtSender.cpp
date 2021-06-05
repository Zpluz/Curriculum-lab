#include "stdafx.h"
#include "Global.h"
#include "GBNRdtSender.h"
#include <vector>

GBNRdtSender::GBNRdtSender() :base(0), nextseqnum(0), isfull(false) {}

GBNRdtSender::~GBNRdtSender() {}

bool GBNRdtSender::getWaitingState()
{
	return isfull;
}

bool GBNRdtSender::send(const Message& msg)
{
	if (this->isfull) { //发送窗口已满
		return false;
	}
	
	//如果发送窗口未满
	Packet curPkt;
	curPkt.acknum = -1;			//制作报文
	curPkt.checksum = 0;
	curPkt.seqnum = this->nextseqnum % encodeBits;
	memcpy(curPkt.payload, msg.data, sizeof(msg.data));
	curPkt.checksum = pUtils->calculateCheckSum(curPkt);

	pUtils->printPacket("发送方发送报文", curPkt);
	pns->sendToNetworkLayer(RECEIVER, curPkt); //将数据包发送到网络层


	if (snd_pkt.size() == 0)
		pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);  //启用发送方计时器

	snd_pkt.push_back(curPkt); //将报文加入窗口
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

void GBNRdtSender::receive(const Packet& ackPkt)
{
	int ackCheckSum = pUtils->calculateCheckSum(ackPkt);
	int ackInWnd = -1;
	int count = 0;
	for (vector<Packet>::iterator iter  = snd_pkt.begin(); iter != snd_pkt.end(); ++iter)
	{
		if ((*iter).seqnum == ackPkt.acknum)
		{
			ackInWnd = count; //计算ack代表的数据包在snd_pkt中的下标
			break;
		}
		count++;
	}
	if (ackCheckSum == ackPkt.checksum && ackInWnd >= 0) //如果ACK在窗口内
	{
		pUtils->printPacket("发送方正确收到接收方发送的ACK", ackPkt);
		for (int i = 0; i <= ackInWnd; ++i)
		{
			pns->stopTimer(SENDER, snd_pkt[i].seqnum); //若3为base，ACK3返回丢失而ACK5先到，此时需要将包括5及之前的所有启动定时器停止，保证3不会超时，再对6启动计时器;
			//若ACK3先超时，则会对3重启计时器，当4及以后的ACK到达时，会关闭3的计时器，从而保证之后不会再次超时
		}
		snd_pkt.erase(snd_pkt.begin(), snd_pkt.begin() + ackInWnd + 1); //更新窗口内分组，从base开始删除掉两个ACK之间的分组数量，包括最新收到的ACK对应分组
		

		this->base = (ackPkt.acknum + 1) % encodeBits;
		this->isfull = (this->snd_pkt.size() < windowSize) ? false : true; //base产生更新,isfull更新

		if (this->snd_pkt.size() != 0)
		{
			int isBaseSend = 0;
			for (vector<Packet>::iterator iter = snd_pkt.begin(); iter != snd_pkt.end(); ++iter)
			{
				if (this->base == iter->seqnum)
				{
					isBaseSend = 1;
					break;
				}
			}
			if (isBaseSend)
			{
				pns->startTimer(SENDER, 6 * (ackInWnd + 1), (this->base) % encodeBits); //计时器始终对最早的已发送但未被确认的分组(base)计时
				//由于一般这个时候的base已经发出去且发出去时并未开启计时器，若直接按原定超时时间(Configuration::TIME_OUT)开始计时则相当于重新刷新其超时时间，不合理，所以此时开始对base计时的超时时间应该适量缩短
			}
			else
			{
				pns->startTimer(SENDER, Configuration::TIME_OUT, (this->base) % encodeBits);
			}
			
			
		}

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
	else
	{
		pUtils->printPacket("当前ACK未在发送方窗口内，忽略该ACK", ackPkt);
	}
}

void GBNRdtSender::timeoutHandler(int seqNum)
{
	pns->stopTimer(SENDER, seqNum);
	for (vector<Packet>::iterator iter = snd_pkt.begin(); iter != snd_pkt.end(); ++iter)
	{
		pUtils->printPacket("发送方定时器时间到，从base开始重发至nextseqnum-1",(*iter));
		pns->sendToNetworkLayer(RECEIVER, (*iter));
	}
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
}