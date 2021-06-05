#include "stdafx.h"
#include "Global.h"
#include "SRRdtReceiver.h"
#include <vector>


SRRdtReceiver::SRRdtReceiver()
{
	for (int i = 0; i < windowSize; ++i)
	{
		expSeqnum.push_back(i % encodeBits); //Ԥ�ڴ��ڳ�ʼ��0,1,2,3
	}
}
SRRdtReceiver::~SRRdtReceiver() {}

void SRRdtReceiver::receive(const Packet& packet)
{
	int pktCheckSum = pUtils->calculateCheckSum(packet);
	int inExpWnd = 0; //�Ƿ���Ԥ�ڴ�����
	int inRcvWnd = -1; //�Ƿ��ѱ�����
	if (pktCheckSum == packet.checksum)
	{
		pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���", packet);
		Packet ackPkt;				//�����Ƿ���Ԥ�ڴ����ڣ���Ҫ����ACK
		ackPkt.seqnum = -1;
		ackPkt.checksum = 0;
		ackPkt.acknum = packet.seqnum;
		for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++)
		{
			ackPkt.payload[i] = '.';
		}
		ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
		pUtils->printPacket("���շ�����ȷ�ϱ���", ackPkt);
		pns->sendToNetworkLayer(SENDER, ackPkt);


		for (vector<int>::iterator itern = expSeqnum.begin(); itern != expSeqnum.end(); ++itern)
		{
			if (packet.seqnum == (*itern)) //˵���ð���Ԥ�ڴ�����
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
				if (inExpWnd && inRcvWnd == -1)  //˵������Ԥ�ڴ�������û�б�����
				{
					rcv_pkt.push_back(packet); //����ð�
				}
				break;
			}
		}

		std::cout << "���հ�" << packet.seqnum << "��Ԥ�ڽ��մ���Ϊ:[ ";
		for (vector<int>::iterator iter = expSeqnum.begin(); iter != expSeqnum.end(); ++iter)
		{
			std::cout << *iter << ' ';
		}
		std::cout << "]�����շ����������е�����Ϊ:[ ";
		for (vector<Packet>::iterator iter = rcv_pkt.begin(); iter != rcv_pkt.end(); ++iter)
		{
			std::cout << iter->seqnum << ' ';
		}
		for (int i = 0; i < windowSize - (int)rcv_pkt.size(); ++i)
		{
			std::cout << '#' << ' ';
		}
		std::cout << "]" << std::endl;

		if (inExpWnd == 1)  //�����յ��İ���Ԥ�ڴ�����
		{
			if (packet.seqnum == (*expSeqnum.begin())) //�����յ��İ�ΪԤ�ڴ����ڵ�base,������������ѻ�����ϴ�
			{
				int expNextBase = 0; //Ԥ�ڵ���һ��Base�����ڸ���expSeqNum
				for (vector<int>::iterator itern = expSeqnum.begin(); itern != expSeqnum.end(); ++itern) 
				{
					int continuous = 0;
					for (vector<Packet>::iterator iterr = rcv_pkt.begin(); iterr != rcv_pkt.end() ; ++iterr)
					{
						if (iterr->seqnum == (*itern)) 
						{
							continuous = 1;
							Message msg;
							memcpy(msg.data, iterr->payload, sizeof(iterr->payload));
							pns->delivertoAppLayer(RECEIVER, msg);
							rcv_pkt.erase(iterr); //���ϴ��İ���rcv_pkt��ɾ��
							break;
						}
					}
					if (!continuous) //�˴������������������ж�
						break;
					expNextBase++;
				}

				//�˴�����expSeqNum
				int lastSeq = expSeqnum.back();
				expSeqnum.erase(expSeqnum.begin(), expSeqnum.begin() + expNextBase);
				while (expSeqnum.size() < windowSize)
				{
					expSeqnum.push_back((lastSeq + 1) % encodeBits); //Ԥ�ڴ��ڴ�expNextBase��ʼ����
					lastSeq = expSeqnum.back();
				}

				std::cout << "���ڻ�����Ԥ�ڽ��մ��ڵ�����Ϊ:[ ";
				for (vector<int>::iterator iter = expSeqnum.begin(); iter != expSeqnum.end(); ++iter)
				{
					std::cout << *iter << ' ';
				}
				std::cout << "]�����շ����������е�����Ϊ:[ ";
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
		}
	}
	
}


