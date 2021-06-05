#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <thread>
#include <errno.h>
#include "server.h"

#pragma comment(lib,"ws2_32.lib")

#define WAIT_LIST 20 //�ȴ����г���


using namespace std;

char* signal = new char[4];
//string parserRequest(char* reqbuf);
//string GetType(string reqFile);
//string GetTime();
//string FileMeasure(string reqFile);
//int FileSend(string reqFile, SOCKET clntskt, string dir);
int process(SOCKET clntskt, string dir);
void close_main(char* argv);


int main(int argc, char* argv[])
{
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) // ��ʼ��DLL,Windows Socket API�汾2.2
	{
		cout << "WSAStartup failed" << endl;
		return -1;
	}
	cout << "Socket API ��ʼ���ɹ�..." << endl;

	SOCKET servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //����TCP��ʽ�׽���

	if (servSock == INVALID_SOCKET)
	{
		cout << "����Socket����ʧ��..." << endl;
		return -1;
	}
	cout << "����Socket�����ɹ�..." << endl;

	//���׽���
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	string ip = "0";
	cout << "��IP��ַ(localhost):";
	getline(cin,ip);
	if (ip == "")
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		inet_pton(AF_INET, ip.c_str(), &sockAddr.sin_addr.s_addr);

	string port = "0";
	cout << "�󶨶˿ں�(80):";
	getline(cin, port);
	if (port == "")
		sockAddr.sin_port = htons(80); //http�˿�
	else
	{
		int pt = stoi(port);
		cout << pt << endl;
		sockAddr.sin_port = htons(pt);
	}

	string dir;
	cout << "�趨�ļ�·��:";
	getline(cin, dir);
	if (dir == "\r")
		dir = ".\\";


	int bind_code = bind(servSock, (SOCKADDR*) &sockAddr, sizeof(sockAddr));
	if (bind_code == SOCKET_ERROR)
	{
		cout << "����Socket��ʧ��..." << endl;
	}
	cout << "����Socket�󶨳ɹ�..." << endl;
	//�������״̬
	if (listen(servSock, WAIT_LIST) == SOCKET_ERROR)
	{
		cout << "����Socket����ʧ��..." << endl;
	}
	cout << "����Socket�����ɹ�..." << endl;


	//�������ܿͻ�������
	sockaddr_in clntAddr;
	int nSize = sizeof(SOCKADDR);
	while (true) //������Socket��������ʱ
	{
		//char choice = 'N';
		//cout << "\n�����Ƿ��˳�(Y/N)?" << endl;
		//cin >> choice;
		//if (choice == 'Y' || choice == 'y')
		//{
		//	break;
		//}

		int nSize = sizeof(SOCKADDR);
		cout << "\n�ȴ�����" << endl;
		SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
		if (clntSock == INVALID_SOCKET)
		{
			cout << "����Socket����ʧ��" << endl;
			continue;
		}
		char ipbuf[20];
		inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, ipbuf, sizeof(ipbuf));
		cout << "����Socket�����ɹ�" << endl;
		cout << "IP:";
		printf("%s\n", ipbuf);
		cout << "Port:" << clntAddr.sin_port << endl;
		
		//��ͻ��˷�������
		if (process(clntSock,dir) == -1)
		{
			cout << "���������" << endl;
		}
		//thread clnt_thrd(process,clntSock,dir);//�����߳�
		//clnt_thrd.detach(); //���߳��������̣߳�������
		cout << endl;
	}
	
	//�ر����׽���
	closesocket(servSock);

	//��ֹDLL��ʹ��
	WSACleanup();
	return 0;
}