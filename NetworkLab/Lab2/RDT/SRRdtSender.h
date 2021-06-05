#ifndef SR_RDT_SENDER_H
#define SR_RDT_SENDER_H

#include "RdtSender.h"
#include <vector>

class SRRdtSender : public RdtSender
{
private:
	int base;						//�����
	int nextseqnum;					//��һ�����
	bool isfull;					//�жϴ����Ƿ�����
	std::vector<Packet> snd_pkt;			//�ѷ��͵����ݰ�����
	std::vector<int> cf_pkt;	//�ѻ���İ����

public:
	SRRdtSender();
	virtual ~SRRdtSender();

public:
	bool send(const Message& message);				//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(const Packet& ackPkt);				//����ȷ��Ack������NetworkServiceSimulator����	
	void timeoutHandler(int seqNum);				//Timeout handler������NetworkServiceSimulator����
	bool getWaitingState();
};

#endif