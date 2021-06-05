#include "stdafx.h"
#include "Global.h"
#include "TCPRdtReceiver.h"

#include <vector>

TCPRdtReceiver::TCPRdtReceiver() :expectedSeqNum(0)
{
	lastAckPkt.acknum = -1; //��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ-1��ʹ�õ���һ�����ܵ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//���Ը��ֶ�
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++)
	{
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
	for (int i = 0; i < windowSize; ++i)
	{
		expSeqnum.push_back(i % encodeBits); //Ԥ�ڴ��ڳ�ʼ��0,1,2,3
	}
}

TCPRdtReceiver::~TCPRdtReceiver() {}

void TCPRdtReceiver::receive(const Packet& packet)
{
	int pktCheckSum = pUtils->calculateCheckSum(packet);
	int inExpWnd = 0; //�Ƿ���Ԥ�ڴ�����
	int inRcvWnd = -1; //�Ƿ��ѱ�����
	if (pktCheckSum == packet.checksum)
	{
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

		if (inExpWnd == 1) //���ڴ�����
		{
			if (packet.seqnum == (*expSeqnum.begin())) //�����յ��İ�ΪԤ�ڴ����ڵ�base,������������ѻ�����ϴ�
			{
				int expNextBase = 0; //Ԥ�ڵ���һ��Base�����ڸ���expSeqNum
				for (vector<int>::iterator itern = expSeqnum.begin(); itern != expSeqnum.end(); ++itern)
				{
					int continuous = 0;
					for (vector<Packet>::iterator iterr = rcv_pkt.begin(); iterr != rcv_pkt.end(); ++iterr)
					{
						if (iterr->seqnum == (*itern))
						{
							this->lastAckPkt.acknum = iterr->seqnum;
							this->lastAckPkt.checksum = pUtils->calculateCheckSum(this->lastAckPkt);
							pUtils->printPacket("���շ�����ȷ�ϱ���", this->lastAckPkt);
							pns->sendToNetworkLayer(SENDER, this->lastAckPkt);

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
			else
			{
				pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����ʧ���ѱ�����", packet);
				pUtils->printPacket("���շ����·����ϴε�ȷ�ϱ���", this->lastAckPkt);
				pns->sendToNetworkLayer(SENDER, this->lastAckPkt); //�ظ�������һ�ν��յ���ACK
			}
		}
		else
		{
			//����Ԥ�ڴ����ڣ�ҲҪ����ACK
			//�п��ܷ���ACK�������ط���������ϴ���Ӧ�ò㵼�´���ǰ��ʹ���ط��İ��Ų��ڴ����У����ͷ����ղ���ACK��ֹͣ��ʱ���ͻ᲻�ϳ�ʱ�ش���������Ҫ�ٷ�ACK��ʹ���ͷ�������ǰ�ƶ�
			Packet ackPkt;				
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
		}
		
	}
	else
	{
		pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
	}
}

