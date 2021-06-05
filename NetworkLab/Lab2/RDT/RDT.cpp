#include "stdafx.h"
#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "GBNRdtSender.h"
#include "GBNRdtReceiver.h"
#include "SRRdtSender.h"
#include "SRRdtReceiver.h"
#include "TCPRdtSender.h"
#include "TCPRdtReceiver.h"
int main()
{
	//RdtSender* ps = new GBNRdtSender();
	//RdtReceiver* pr = new GBNRdtReceiver();
	//RdtSender* ps = new SRRdtSender();
	//RdtReceiver* pr = new SRRdtReceiver();
	RdtSender* ps = new TCPRdtSender();
	RdtReceiver* pr = new TCPRdtReceiver();
	pns->setRunMode(0);  //VERBOSģʽ
	//pns->setRunMode(1);  //����ģʽ
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("C:\\Users\\Zilai Zeng\\source\\course\\NetworkLab\\Lab2\\input.txt");
	pns->setOutputFile("C:\\Users\\Zilai Zeng\\source\\course\\NetworkLab\\Lab2\\output.txt");
	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
	delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete

	return 0;
}
