#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma  comment(lib, "Ws2_32.lib")

DWORD WINAPI WorkThread(void* param)
{
	SOCKET* psClient = (SOCKET*)param;
	char buf[4096];
	while (true)
	{
		int len = ::recv(*psClient, buf, 4096, 0);//�������ã���������
		if (len <= 0)
		{
			printf("recvʧ�ܣ�%d\n", WSAGetLastError());
			Sleep(5000);
			break;
		}
		buf[len] = '\0';
		printf("�յ�����:%s\n", buf);
	}
	::closesocket(*psClient);//�ر�socket
	delete psClient;
	return 0;
}

int main()
{
	WSAData wsaData;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))//win socket dll��ʼ��
	{
		printf("WSAStartupʧ�ܣ�\n", WSAGetLastError());
		Sleep(5000);
		return 0;
	}
	USHORT nPort{ 3456 };
	//tcp, udp��AF_INET��ַ��,SOCK_STREAM��ʾ������,IPPROTO_TCP��ʾ��tcpЭ��
	//��ʼ��sockect
	SOCKET sListen = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//��ʼ��������ַ
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(nPort);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;

	//�󶨼����˿�
	if (SOCKET_ERROR == ::bind(sListen, (sockaddr*)&sin, sizeof(sin)))
	{
		printf("bindʧ�ܣ�%d\n", WSAGetLastError());
		Sleep(5000);
		return -1;
	}

	//��ʼ����
	::listen(sListen, 5);

	while (true)//����������һ����ѭ��������
	{
		sockaddr_in addrRemote;
		int nAddrLen = sizeof(addrRemote);
		SOCKET* psClient = new SOCKET;
		*psClient = ::accept(sListen, (sockaddr*)&addrRemote, &nAddrLen);
		HANDLE hThread = CreateThread(NULL, 0, WorkThread, psClient, 0, NULL);
		CloseHandle(hThread);
	}
	closesocket(sListen);
	WSACleanup();//���������cleanup��������

	system("Pause");

}