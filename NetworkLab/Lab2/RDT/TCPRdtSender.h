#ifndef TCP_RDT_SENDER
#define TCP_RDT_SENDER

#include <vector>
#include "RdtSender.h"

class TCPRdtSender :public RdtSender
{
private:
	int base;						//基序号
	int nextseqnum;					//下一个序号
	bool isfull;					//判断窗口是否已满
	int lastACK;					//上一次接收的ACK序号
	std::vector<Packet> snd_pkt;			//已发送的数据包数组

public:
	bool send(const Message& message);						//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
	void receive(const Packet& ackPkt);						//接受确认Ack，将被NetworkServiceSimulator调用	
	void timeoutHandler(int seqNum);					//Timeout handler，将被NetworkServiceSimulator调用
	bool getWaitingState();

public:
	TCPRdtSender();
	virtual ~TCPRdtSender();
};
#endif