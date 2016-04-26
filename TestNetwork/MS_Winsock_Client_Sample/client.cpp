#define WIN32_LEAN_AND_MEAN

//#include <windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int __cdecl main(int argc, char **argv)
{
	WSADATA wsaData;
	SOCKET connectSocket = INVALID_SOCKET;
	struct addrinfo* result = NULL;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;//c�������stuct,c++�Ͳ�����
	char* sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	if (argc != 2)
	{
		printf("usage: %s server-name\n", argv[0]);
		return 1;
	}

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error:%d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;//Ϊʲô��δָ����ֻ�з�������ָ����
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//The getaddrinfo function provides protocol - independent translation from an ANSI host name to an address.
	//unicode�汾��Ӧ����ΪGetAddrInfoW
	//�൱����һ�ν��ܣ�https://msdn.microsoft.com/en-us/library/ms738520
	//��һ������Ϊ����������ip��ַ���ڶ�������Ϊ������������http������80�˿ڣ�,��˿ں�
	//������������������ȡ���ĸ�������һ��hint,���ĸ�������һ��addrinfo����ָ��
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error:%d\n", iResult);
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	//������������֧�ֵĵ�ַ��Ϣ��
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (connectSocket == INVALID_SOCKET)
		{
			printf("socket failed with error:%d\n", iResult);
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}

		iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(connectSocket);
			connectSocket = INVALID_SOCKET;
			continue;//���Ӳ��ɹ������
		}
		break;//���ӳɹ����˳�
	}

	//�Ѿ�����Ҫ��ַ��Ϣ�ˣ��ͷ��ڴ�
	freeaddrinfo(result);

	if (connectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}
	iResult = send(connectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR)
	{
		printf("send failed with error:%d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent:%ld\n", iResult);

	//shutdown�����ٷ�������
	iResult = shutdown(connectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error:%d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	do 
	{
		iResult = recv(connectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			printf("Byte received:%d\n", iResult);
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error:%d\n", WSAGetLastError());
	} while (iResult > 0);

	closesocket(connectSocket);
	WSACleanup();
	system("Pause");

	return 0;
}