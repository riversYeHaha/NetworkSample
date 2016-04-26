#undef UNICODE
#define WIN32_LEAN_AND_MEAN

//#include <windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int __cdecl main(void)
{
	WSADATA wsaData;
	int iResult;

	SOCKET listenSockect = INVALID_SOCKET;
	SOCKET clientSocket = INVALID_SOCKET;

	struct addrinfo* result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed width error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error:%d\n", iResult);
		WSACleanup();
		return 1;
	}

	listenSockect = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listenSockect == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = bind(listenSockect, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error:%d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(listenSockect);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(listenSockect, SOMAXCONN);//SOMAXCONN指定最大的队列长度
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error:%d\n", WSAGetLastError());
		closesocket(listenSockect);
		WSACleanup();
		return 1;
	}

	clientSocket = accept(listenSockect, NULL, NULL);
	if (clientSocket == INVALID_SOCKET)
	{
		printf("accept failed with error:%d\n", WSAGetLastError());
		closesocket(listenSockect);
		WSACleanup();
		return 1;
	}

	//不再需要服务端socket
	closesocket(listenSockect);
	do {
		iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			printf("Bytes received:%d\n", iResult);

			iSendResult = send(clientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR)
			{
				printf("send failed with error:%d\n", WSAGetLastError());
				closesocket(listenSockect);
				WSACleanup();
				return 1;
			}
			printf("Byte sent:%d\n", iResult);
		}
		else if (iResult == 0)
		{
			printf("Connection closing...\n");
		}
		else
		{
			printf("recv failed with error:%d\n", WSAGetLastError());
			closesocket(listenSockect);
			WSACleanup();
			return 1;
		}
	} while (iResult > 0);

	//为什么需要shutdown
	iResult = shutdown(clientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	closesocket(clientSocket);
	WSACleanup();
	system("Pause");

	return 0;
}