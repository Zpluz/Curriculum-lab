#ifndef SR_RDT_RECEIVER_H
#define SR_RDT_RECEIVER_H

#include "RdtReceiver.h"
#include <vector>
class SRRdtReceiver : public RdtReceiver
{
private:
	std::vector<int> expSeqnum;				//预期接收的包序号
	std::vector<Packet> rcv_pkt;			//已缓存的数据包数组

public:
	void receive(const Packet& packet); //接收报文，将被NetworkService调用

public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();

};

#endif