// TCPServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <thread>
#include <WinSock2.h>	// 2.2 版本

#pragma comment(lib, "ws2_32.lib")	// 网络编程需要的链接库（最新的版本号）
#pragma warning(disable : 4996)

using namespace std;

// 线程回调函数（接收客户端的信息）
DWORD CALLBACK ThreadProc(LPVOID lp)	// lp是接收到的最新的套接字
{
	SOCKET 	NewConnection = (SOCKET)lp;	// 转型
	char	DataBuffer[1024];		// 用于存储接收到的数据
	int 	Ret;				// 用于判断

	while (1)	// 一直要接收这个客户端的数据
	{
		// 接收数据并判断是否成功，该函数返回接收字符个数
		if ((Ret = recv(NewConnection, DataBuffer, sizeof(DataBuffer), 0)) == 0)
		{
			// 返回错误数据
			printf("recv failed with error %d\n", WSAGetLastError());
			closesocket(NewConnection);		// 释放套接字
			WSACleanup();			// 清空启动信息
			return 0;
		}
				
		if(Ret < 0)
		{
			printf("exit a thread. errorCode = %d \n", WSAGetLastError());
			break;
		}

		printf("We successfully received %d bytes.\n", Ret);	// 打印接收了多少数据
		if(Ret>= 0 && Ret < 1024)
			DataBuffer[Ret] = '\0';		// 接收的数据是没有 \0的

		if (Ret % 2 == 0)
		{
			cout << "data: ";
			for (int i = 0; i < Ret; i++)
			{
				cout << " " << hex << setw(2) << setfill(' ') << uppercase << (int)DataBuffer[i];
			}
			cout << endl;
		}
		else
			printf("%s\n", DataBuffer);	// 打印数据

		Sleep(5); // 防止占用过高 CPU
	}

	closesocket(NewConnection);		// 释放套接字	    
}

int main()
{
	std::cout << "Hello World!\n";

	WSADATA		wsaData;		// 一种用于初始启动信息的数据结构，加载winsocket.dll
	SOCKET		ListeningSocket;// 服务端的套接字
	SOCKET		NewConnection;	// 接受到的客户端的套接字
	SOCKADDR_IN		ServerAddr;	// 服务端地址
	SOCKADDR_IN		ClientAddr;	// 客户端地址
	int			ClientAddrLen;	// 客户端地址长度
	int 		Port = 9999;	// 端口号（不能是太小的数，可以被其它程序所用）
	int         Ret = 0;

	// 初始启动信息（加载版本号，模块），并判断是否成功
	if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		printf("WSAStartup failed with error %d\n", Ret);
		return -1;
	}

	// 创建Socket, 使用 IPv4 进行通信（Windows只有AF_INET)、流式套接字、TCP协议
	if ((ListeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		printf("socket failed with error %d\n", WSAGetLastError());
		WSACleanup();	// 与 WSAStartup 配套使用
		return -1;
	}

	// 初始化地址信息
	ServerAddr.sin_family = AF_INET;	// 地址格式
	ServerAddr.sin_port = htons(Port);	// 地址端口号（字节转换成网络大端模式）
	ServerAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);	// 接受本机内的所有ip地址（长整形，转化字节）

	// 绑定地址，并判断是否成功
	if ((bind(ListeningSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR))
	{
		printf("bind failed with error %d\n", WSAGetLastError());
		closesocket(ListeningSocket);	// 释放套接字
		WSACleanup();
		return -1;
	}

	// 监听请求信号，最多有五个客户端（自己设置，不要太多），并判断是否成功
	if (listen(ListeningSocket, 5) == SOCKET_ERROR)
	{
		printf("listen failed with error %d\n", WSAGetLastError());
		closesocket(ListeningSocket);
		WSACleanup();
		return -1;
	}

	// 提示信息
	printf("We are waiting a connection on port %d.\n", Port);
	printf("Listen(正在监听)...\n");

	while (1)	// 一直等待客户端连接
	{
		ClientAddrLen = sizeof(SOCKADDR);	// 求出地址的长度
		// 接收请求（链接客户端），第二个参数是客户端的地址
		if ((NewConnection = accept(ListeningSocket, (SOCKADDR*)&ClientAddr, &ClientAddrLen)) == INVALID_SOCKET)
		{
			printf("accept failed with error %d\n", WSAGetLastError());
			closesocket(ListeningSocket);
			WSACleanup();
			return -1;
		}

		// 链接客户端成功，打印客户端的ip地址和端口号，（ip地址转换和字节转换）
		printf("We successfully got a connection from %s:%d\n", inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));

		printf("We are waiting to receive data\n");

		// ******************创建多线程******************
		//CreateThread(NULL, 0, ThreadProc, (LPVOID)NewConnection, 0, NULL);
		thread th(ThreadProc, (LPVOID)NewConnection);
		th.detach(); // 分离 不影响主线程接收新客户端请求

		Sleep(1000);
	}

	printf("We are now going to close the client connection.\n");
	closesocket(NewConnection);
	WSACleanup();

	system("pause");

	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
