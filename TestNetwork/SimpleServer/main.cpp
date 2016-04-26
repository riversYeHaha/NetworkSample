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
		int len = ::recv(*psClient, buf, 4096, 0);//阻塞调用，接收数据
		if (len <= 0)
		{
			printf("recv失败！%d\n", WSAGetLastError());
			Sleep(5000);
			break;
		}
		buf[len] = '\0';
		printf("收到数据:%s\n", buf);
	}
	::closesocket(*psClient);//关闭socket
	delete psClient;
	return 0;
}

int main()
{
	WSAData wsaData;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))//win socket dll初始化
	{
		printf("WSAStartup失败！\n", WSAGetLastError());
		Sleep(5000);
		return 0;
	}
	USHORT nPort{ 3456 };
	//tcp, udp用AF_INET地址簇,SOCK_STREAM表示流数据,IPPROTO_TCP表示用tcp协议
	//初始化sockect
	SOCKET sListen = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//初始化监听地址
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(nPort);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;

	//绑定监听端口
	if (SOCKET_ERROR == ::bind(sListen, (sockaddr*)&sin, sizeof(sin)))
	{
		printf("bind失败！%d\n", WSAGetLastError());
		Sleep(5000);
		return -1;
	}

	//开始监听
	::listen(sListen, 5);

	while (true)//服务器就是一个死循环。。。
	{
		sockaddr_in addrRemote;
		int nAddrLen = sizeof(addrRemote);
		SOCKET* psClient = new SOCKET;
		*psClient = ::accept(sListen, (sockaddr*)&addrRemote, &nAddrLen);
		HANDLE hThread = CreateThread(NULL, 0, WorkThread, psClient, 0, NULL);
		CloseHandle(hThread);
	}
	closesocket(sListen);
	WSACleanup();//如果不调用cleanup会怎样？

	system("Pause");

}