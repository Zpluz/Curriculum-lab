#ifndef GBN_RDT_SENDER_H
#define GBN_RDT_SENDER_H


#include <vector>
#include "RdtSender.h"

class GBNRdtSender : public RdtSender
{
private:
	int base;						//�����
	int nextseqnum;					//��һ�����
	bool isfull;					//�жϴ����Ƿ�����
	std::vector<Packet> snd_pkt;			//�ѷ��͵����ݰ�����

public:
	bool send(const Message& message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(const Packet& ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����	
	void timeoutHandler(int seqNum);					//Timeout handler������NetworkServiceSimulator����
	bool getWaitingState();

public:
	GBNRdtSender();
	virtual ~GBNRdtSender();

};


#endif