#include <time.h>
#include <winsock2.h>
#include <string>
#include <fstream>
#include <iostream>

#define REQ_SIZE 8192 //���ջ�����8K
#define STATUS_LENGTH 30 //��Ӧ״̬����

using namespace std;

string parserRequest(char* reqbuf) //��http�������еõ������ļ�
{
	string request(reqbuf);
	size_t pos = 0;
	size_t end = 0;
	string reqfile;
	pos = request.find("GET /");
	end = request.find("HTTP/", pos + 5);
	reqfile = request.substr(pos + 5, end - (pos + 5) - 1);
	return reqfile;
}


string GetType(string reqFile)  //�������ļ��л�ȡ�ļ�����
{
	size_t type_pos = reqFile.find('.');
	string type = reqFile.substr(type_pos + 1);
	string typehd;
	if (type == "html" || type == "plain" || type == "xml")
	{
		typehd = "Content-Type: text/" + type + " ;charset=utf-8" + "\r\n";
	}
	else if (type == "jpeg" || type == "png" || type == "gif" || type == "jpg")
	{
		if (type == "jpg")
		{
			typehd = "Content-Type: image/jpeg \r\n";
		}
		else if (type == "ico")
		{
			typehd = "Content-Type: image/x-icon \r\n";
		}
		else
			typehd = "Content-Type: image/" + type + "\r\n";
	}
	else if (type == "xml" || type == "json" || type == "pdf" || type == "octet-stream")
	{
		typehd = "Content-Type: application/" + type + "\r\n";
	}
	else if(type == "mp4")
	{
		typehd = "Content-Type: video/mpeg4\r\n";
	}
	cout << typehd;
	return typehd;
}



string GetTime() //�õ�GMTʱ��
{
	time_t tn = time(nullptr);
	tm* gmt = gmtime(&tn);
	const char* fmt = "%a, %d %b %Y %H:%M:%S GMT";
	char gmtstr[30];

	strftime(gmtstr, sizeof(gmtstr), fmt, gmt);
	string timestr = gmtstr;
	string timehd = "Date:" + timestr + "\r\n"; \
	cout << timehd;
	return timehd;
}



string FileMeasure(string reqFile)
{
	ifstream localFile;
	localFile.open(reqFile, ios::in | ios::binary);
	localFile.seekg(0, ios::end);
	int length = (int)localFile.tellg();
	localFile.seekg(0, ios::beg);
	string lengthhd = "Content-Length: " + to_string(length) + "\r\n";
	cout << lengthhd;
	return lengthhd;
}



int FileSend(string reqFile, SOCKET clntskt, string dir) //�õ������ļ��Ĵ�С
{
	ifstream localFile;
	localFile.open(dir + reqFile, ios::in | ios::binary);
	localFile.seekg(0, ios::end);
	int length = (int)localFile.tellg();
	localFile.seekg(0, ios::beg);
	int buffer_size = 1024;
	if (length > 1024000)
		buffer_size = 20480; //���ͻ�����20K
	else
		buffer_size = 8192; //���ͻ�����8K
	int cycles = (length / buffer_size) + 1;
	int transdata = 0;  //ͳ�����ݴ�����
	char* buffer = new char[buffer_size];
	for (int i = 0; i < cycles && localFile; ++i)
	{
		int send_length = (int)localFile.read(buffer, buffer_size).gcount();
		transdata += send_length;
		int send_actual_size = send(clntskt, buffer, send_length, 0);
		if (send_actual_size == -1)
		{
			int wc = WSAGetLastError();
			cout << "�����ļ����ʹ�����:" << wc << endl;
			return -1;
		}
		cout << "��ǰ����������Ϊ:" << transdata << "  " << "���跢��:"<< length - transdata << endl;
	}
	delete[] buffer;
	localFile.close();
	return 0;
}



int process(SOCKET clntskt, string dir) //����client����
{
	int sc = 0;
	cout << "��ǰ����Socket:" << clntskt << endl;
	char OK_status[STATUS_LENGTH] = "HTTP/1.1 200 OK\r\n";
	char ERR_status[STATUS_LENGTH] = "HTTP/1.1 404 Not Found\r\n";
	char BAD_status[STATUS_LENGTH] = "HTTP/1.1 400 Bad Request\r\n";
	char reqBuffer[REQ_SIZE] = { 0 };
	sc = recv(clntskt, reqBuffer, REQ_SIZE, 0); //��������
	if (sc == SOCKET_ERROR)
	{
		if (errno != EWOULDBLOCK)
		{
			perror("����ʧ��");
			return -1;
		}
	}
	cout << reqBuffer << endl;
	string reqFile = parserRequest(reqBuffer); //��ȡ�����ļ�
	cout << "�����ļ�:" << reqFile << endl;

	size_t type_pos = reqFile.find('.');
	string type = reqFile.substr(type_pos + 1);
	string name = reqFile.substr(0, type_pos);

	int reqtypeNum = 0;
	if (type != "html" && type != "jpg" && type != "jpeg" && type != "gif" && type != "mp4") 
	{
		reqtypeNum = 1;
	}
	if (reqtypeNum == 1)
	{
		cout << "�ļ��������..." << endl;
		cout << "400 Bad Request" << endl;
		closesocket(clntskt);
		return -1;
	}
	else
	{
		ifstream fin(reqFile);
		if (!fin)
		{
			cout << "�ļ�δ�ҵ�..." << endl;
			cout << "404 Not Found" << endl;
			string ERR_header = ERR_status + FileMeasure("404.html") + GetType("404.html") + GetTime(); //���屨ͷӦ�ð���״̬�У�ChromeҪ��״̬�к�������ͷ����һ��,���ֿܷ���
			if (send(clntskt, ERR_header.c_str(), ERR_header.size(), 0) == -1)
			{
				cout << "��Ӧ��ͷ���ʹ���..." << endl;
				int wc = WSAGetLastError();
				cout << "��Ӧ��ͷ���ʹ�����:" << wc << endl;
			}
			if (FileSend("404.html", clntskt, dir) == -1)
			{
				cout << "�����ļ����ʹ���..." << endl;
			}
			cout << "������ʧ��" << endl;
			cout << "Socket:" << clntskt << "����" << endl;
			cout << endl;
			closesocket(clntskt);
			return -1;
		}
		else
		{
			cout << "�ļ����ҵ�����������..." << endl;
			string OK_header = OK_status + FileMeasure(reqFile) + GetType(reqFile) + GetTime() + "\r\n";

			if (send(clntskt, OK_header.c_str(), OK_header.size(), 0) == -1) //������Ӧ��ͷ
			{
				cout << "��Ӧ��ͷ���ʹ���..." << endl;
				int wc = WSAGetLastError();
				cout << "��Ӧ��ͷ���ʹ�����:" << wc << endl;
			}
			cout << "��Ӧ��ͷ���ͳɹ�" << endl;
			if (FileSend(reqFile, clntskt, dir) == -1) //���������ļ�
			{
				cout << "�����ļ����ʹ���..." << endl;
			}
			cout << "������ɹ�" << endl;
			cout << "Socket:" << clntskt << "����" << endl;
			cout << endl;
			closesocket(clntskt);
			return 0;
		}
	}
	

}