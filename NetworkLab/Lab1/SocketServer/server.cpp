#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <thread>
#include <errno.h>
#include "server.h"

#pragma comment(lib,"ws2_32.lib")

#define WAIT_LIST 20 //等待队列长度


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

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) // 初始化DLL,Windows Socket API版本2.2
	{
		cout << "WSAStartup failed" << endl;
		return -1;
	}
	cout << "Socket API 初始化成功..." << endl;

	SOCKET servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //创建TCP流式套接字

	if (servSock == INVALID_SOCKET)
	{
		cout << "监听Socket创建失败..." << endl;
		return -1;
	}
	cout << "监听Socket创建成功..." << endl;

	//绑定套接字
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	string ip = "0";
	cout << "绑定IP地址(localhost):";
	getline(cin,ip);
	if (ip == "")
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		inet_pton(AF_INET, ip.c_str(), &sockAddr.sin_addr.s_addr);

	string port = "0";
	cout << "绑定端口号(80):";
	getline(cin, port);
	if (port == "")
		sockAddr.sin_port = htons(80); //http端口
	else
	{
		int pt = stoi(port);
		cout << pt << endl;
		sockAddr.sin_port = htons(pt);
	}

	string dir;
	cout << "设定文件路径:";
	getline(cin, dir);
	if (dir == "\r")
		dir = ".\\";


	int bind_code = bind(servSock, (SOCKADDR*) &sockAddr, sizeof(sockAddr));
	if (bind_code == SOCKET_ERROR)
	{
		cout << "监听Socket绑定失败..." << endl;
	}
	cout << "监听Socket绑定成功..." << endl;
	//进入监听状态
	if (listen(servSock, WAIT_LIST) == SOCKET_ERROR)
	{
		cout << "监听Socket监听失败..." << endl;
	}
	cout << "监听Socket监听成功..." << endl;


	//并发接受客户端请求
	sockaddr_in clntAddr;
	int nSize = sizeof(SOCKADDR);
	while (true) //当监听Socket正常工作时
	{
		//char choice = 'N';
		//cout << "\n程序是否退出(Y/N)?" << endl;
		//cin >> choice;
		//if (choice == 'Y' || choice == 'y')
		//{
		//	break;
		//}

		int nSize = sizeof(SOCKADDR);
		cout << "\n等待连接" << endl;
		SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
		if (clntSock == INVALID_SOCKET)
		{
			cout << "连接Socket创建失败" << endl;
			continue;
		}
		char ipbuf[20];
		inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, ipbuf, sizeof(ipbuf));
		cout << "连接Socket创建成功" << endl;
		cout << "IP:";
		printf("%s\n", ipbuf);
		cout << "Port:" << clntAddr.sin_port << endl;
		
		//向客户端发送数据
		if (process(clntSock,dir) == -1)
		{
			cout << "请求处理错误" << endl;
		}
		//thread clnt_thrd(process,clntSock,dir);//创建线程
		//clnt_thrd.detach(); //子线程脱离主线程，非阻塞
		cout << endl;
	}
	
	//关闭主套接字
	closesocket(servSock);

	//终止DLL的使用
	WSACleanup();
	return 0;
}