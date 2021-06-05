#ifndef TCP_RDT_RECEIVER_H
#define TCP_RDT_RECEIVER_H

#include "RdtReceiver.h"
#include <vector>
class TCPRdtReceiver : public RdtReceiver
{
private:
	int expectedSeqNum;				//��һ��������յı���
	Packet lastAckPkt;				//�ϴη��͵�ȷ�ϱ���
	std::vector<int> expSeqnum;			//Ԥ�ڽ��յİ����
	std::vector<Packet> rcv_pkt;		//�ѻ�������ݰ�����

public:
	void receive(const Packet& packet); //���ձ��ģ�����NetworkService����

public:
	TCPRdtReceiver();
	virtual ~TCPRdtReceiver();

};

#endif