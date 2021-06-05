#ifndef TCP_RDT_SENDER
#define TCP_RDT_SENDER

#include <vector>
#include "RdtSender.h"

class TCPRdtSender :public RdtSender
{
private:
	int base;						//�����
	int nextseqnum;					//��һ�����
	bool isfull;					//�жϴ����Ƿ�����
	int lastACK;					//��һ�ν��յ�ACK���
	std::vector<Packet> snd_pkt;			//�ѷ��͵����ݰ�����

public:
	bool send(const Message& message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(const Packet& ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����	
	void timeoutHandler(int seqNum);					//Timeout handler������NetworkServiceSimulator����
	bool getWaitingState();

public:
	TCPRdtSender();
	virtual ~TCPRdtSender();
};
#endif