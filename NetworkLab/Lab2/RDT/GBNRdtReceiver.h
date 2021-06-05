#ifndef GBN_RDT_RECEIVER_H
#define GBN_RDT_RECEIVER_H

#include "RdtReceiver.h"
class GBNRdtReceiver : public RdtReceiver
{
private:
	int expectedSeqNum;  //��һ��������յı���
	Packet lastAckPkt;		//�ϴη��͵�ȷ�ϱ���

public:
	void receive(const Packet& packet); //���ձ��ģ�����NetworkService����

public:
	GBNRdtReceiver();
	virtual ~GBNRdtReceiver();

};

#endif