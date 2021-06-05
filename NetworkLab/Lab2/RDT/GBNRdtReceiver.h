#ifndef GBN_RDT_RECEIVER_H
#define GBN_RDT_RECEIVER_H

#include "RdtReceiver.h"
class GBNRdtReceiver : public RdtReceiver
{
private:
	int expectedSeqNum;  //下一个按序接收的报文
	Packet lastAckPkt;		//上次发送的确认报文

public:
	void receive(const Packet& packet); //接收报文，将被NetworkService调用

public:
	GBNRdtReceiver();
	virtual ~GBNRdtReceiver();

};

#endif