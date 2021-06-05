#include "stdafx.h"
#include "Global.h"
#include "TCPRdtSender.h"
#include <vector>

TCPRdtSender::TCPRdtSender() :base(0), nextseqnum(0), isfull(false) {}

TCPRdtSender::~TCPRdtSender() {}

int acmAck = 0; //����ACK����
bool TCPRdtSender::getWaitingState()
{
	return isfull;
}

bool TCPRdtSender::send(const Message& msg)
{
	if (this->isfull) { //���ʹ�������
		return false;
	}

	//������ʹ���δ��
	Packet curPkt;
	curPkt.acknum = -1;			//��������
	curPkt.checksum = 0;
	curPkt.seqnum = this->nextseqnum % encodeBits;
	memcpy(curPkt.payload, msg.data, sizeof(msg.data));
	curPkt.checksum = pUtils->calculateCheckSum(curPkt);

	pUtils->printPacket("���ͷ����ͱ���", curPkt);
	pns->sendToNetworkLayer(RECEIVER, curPkt); //�����ݰ����͵������

	if (snd_pkt.size() == 0)
		pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);  //���÷��ͷ���ʱ��

	snd_pkt.push_back(curPkt); //�����ļ��봰��

	this->nextseqnum++;
	this->nextseqnum %= encodeBits;
	this->isfull = (this->snd_pkt.size() < windowSize) ? false : true; //���и���,isfull����
	std::cout << "��ӱ��ģ����ͷ����������е�����Ϊ:[ ";
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

void TCPRdtSender::receive(const Packet& ackPkt)
{
	int ackCheckSum = pUtils->calculateCheckSum(ackPkt);
	int ackInWnd = -1;
	int count = 0;
	for (vector<Packet>::iterator iter = snd_pkt.begin(); iter != snd_pkt.end(); ++iter)
	{
		if ((*iter).seqnum == ackPkt.acknum)
		{
			ackInWnd = count; //����ack��������ݰ���snd_pkt�е��±�
			break;
		}
		count++;
	}
	if (ackCheckSum == ackPkt.checksum && ackInWnd >= 0) //���ACK�ڴ�����
	{
		pUtils->printPacket("���ͷ���ȷ�յ����շ����͵�ACK", ackPkt);
		for (int i = 0; i <= ackInWnd; ++i)
		{
			pns->stopTimer(SENDER, snd_pkt[i].seqnum); //��3Ϊbase��ACK3���ض�ʧ��ACK5�ȵ�����ʱ��Ҫ������5��֮ǰ������������ʱ��ֹͣ����֤3���ᳬʱ���ٶ�6������ʱ��
			//��ACK3�ȳ�ʱ������3������ʱ������4���Ժ��ACK����ʱ����ر�3�ļ�ʱ�����Ӷ���֤֮�󲻻��ٴγ�ʱ
		}
		snd_pkt.erase(snd_pkt.begin(), snd_pkt.begin() + ackInWnd + 1); //���´����ڷ��飬��base��ʼɾ��������ACK֮��ķ������������������յ���ACK��Ӧ����

		this->lastACK = ackPkt.acknum;
		acmAck = 0; //ÿ�θ���lastACK����Ҫ�������������0
		this->base = (ackPkt.acknum + 1) % encodeBits;
		this->isfull = (this->snd_pkt.size() < windowSize) ? false : true; //base��������,isfull����

		if (this->snd_pkt.size() != 0)
		{
			pns->startTimer(SENDER, Configuration::TIME_OUT, (this->base) % encodeBits); //��ʱ��ʼ�ն�������ѷ��͵�δ��ȷ�ϵķ���(base)��ʱ
		}

		std::cout << "���ڻ��������ͷ����������е�����Ϊ:[ ";
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
		if (ackPkt.acknum == this->lastACK)
		{
			acmAck++;
			pUtils->printPacket("���յ�����ACK", ackPkt);
			if (acmAck == 3)
			{
				pUtils->printPacket("��������ACK�������ش�base", (*this->snd_pkt.begin()));
				pns->sendToNetworkLayer(RECEIVER, (*this->snd_pkt.begin()));
				acmAck = 0; //�������������
			}
		}
	}
}

void TCPRdtSender::timeoutHandler(int seqNum)
{
	pns->stopTimer(SENDER, seqNum);

	pUtils->printPacket("���ͷ���ʱ��ʱ�䵽���ط�base", (*this->snd_pkt.begin()));
	pns->sendToNetworkLayer(RECEIVER, (*this->snd_pkt.begin()));

	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
}