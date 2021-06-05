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
	std::cout << "��ӱ��ģ����ͷ��������ѷ��͵�����Ϊ:[ ";
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
			pns->stopTimer(SENDER, snd_pkt[i].seqnum); //��3Ϊbase��ACK3���ض�ʧ��ACK5�ȵ�����ʱ��Ҫ������5��֮ǰ������������ʱ��ֹͣ����֤3���ᳬʱ���ٶ�6������ʱ��;
			//��ACK3�ȳ�ʱ������3������ʱ������4���Ժ��ACK����ʱ����ر�3�ļ�ʱ�����Ӷ���֤֮�󲻻��ٴγ�ʱ
		}
		snd_pkt.erase(snd_pkt.begin(), snd_pkt.begin() + ackInWnd + 1); //���´����ڷ��飬��base��ʼɾ��������ACK֮��ķ������������������յ���ACK��Ӧ����
		

		this->base = (ackPkt.acknum + 1) % encodeBits;
		this->isfull = (this->snd_pkt.size() < windowSize) ? false : true; //base��������,isfull����

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
				pns->startTimer(SENDER, 6 * (ackInWnd + 1), (this->base) % encodeBits); //��ʱ��ʼ�ն�������ѷ��͵�δ��ȷ�ϵķ���(base)��ʱ
				//����һ�����ʱ���base�Ѿ�����ȥ�ҷ���ȥʱ��δ������ʱ������ֱ�Ӱ�ԭ����ʱʱ��(Configuration::TIME_OUT)��ʼ��ʱ���൱������ˢ���䳬ʱʱ�䣬���������Դ�ʱ��ʼ��base��ʱ�ĳ�ʱʱ��Ӧ����������
			}
			else
			{
				pns->startTimer(SENDER, Configuration::TIME_OUT, (this->base) % encodeBits);
			}
			
			
		}

		std::cout << "���ڻ��������ͷ��������ѷ��͵�����Ϊ:[ ";
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
		pUtils->printPacket("��ǰACKδ�ڷ��ͷ������ڣ����Ը�ACK", ackPkt);
	}
}

void GBNRdtSender::timeoutHandler(int seqNum)
{
	pns->stopTimer(SENDER, seqNum);
	for (vector<Packet>::iterator iter = snd_pkt.begin(); iter != snd_pkt.end(); ++iter)
	{
		pUtils->printPacket("���ͷ���ʱ��ʱ�䵽����base��ʼ�ط���nextseqnum-1",(*iter));
		pns->sendToNetworkLayer(RECEIVER, (*iter));
	}
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
}