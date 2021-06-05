#ifndef SR_RDT_SENDER_H
#define SR_RDT_SENDER_H

#include "RdtSender.h"
#include <vector>

class SRRdtSender : public RdtSender
{
private:
	int base;						//基序号
	int nextseqnum;					//下一个序号
	bool isfull;					//判断窗口是否已满
	std::vector<Packet> snd_pkt;			//已发送的数据包数组
	std::vector<int> cf_pkt;	//已缓存的包序号

public:
	SRRdtSender();
	virtual ~SRRdtSender();

public:
	bool send(const Message& message);				//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
	void receive(const Packet& ackPkt);				//接受确认Ack，将被NetworkServiceSimulator调用	
	void timeoutHandler(int seqNum);				//Timeout handler，将被NetworkServiceSimulator调用
	bool getWaitingState();
};

#endif