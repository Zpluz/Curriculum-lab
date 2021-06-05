#ifndef SR_RDT_RECEIVER_H
#define SR_RDT_RECEIVER_H

#include "RdtReceiver.h"
#include <vector>
class SRRdtReceiver : public RdtReceiver
{
private:
	std::vector<int> expSeqnum;				//Ԥ�ڽ��յİ����
	std::vector<Packet> rcv_pkt;			//�ѻ�������ݰ�����

public:
	void receive(const Packet& packet); //���ձ��ģ�����NetworkService����

public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();

};

#endif