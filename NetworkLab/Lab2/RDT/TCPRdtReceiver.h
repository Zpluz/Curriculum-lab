#ifndef TCP_RDT_RECEIVER_H
#define TCP_RDT_RECEIVER_H

#include "RdtReceiver.h"
#include <vector>
class TCPRdtReceiver : public RdtReceiver
{
private:
	int expectedSeqNum;				//下一个按序接收的报文
	Packet lastAckPkt;				//上次发送的确认报文
	std::vector<int> expSeqnum;			//预期接收的包序号
	std::vector<Packet> rcv_pkt;		//已缓存的数据包数组

public:
	void receive(const Packet& packet); //接收报文，将被NetworkService调用

public:
	TCPRdtReceiver();
	virtual ~TCPRdtReceiver();

};

#endif