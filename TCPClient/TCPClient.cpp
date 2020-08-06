// TCPClient.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")	// 网络编程需要的链接库
#pragma warning(disable : 4996)

using namespace std;

int main()
{
    std::cout << "Hello World!\n";

    WSADATA 		wsaData;	// 用于初始启动信息
    SOCKET		    s;		    // 客户端的套接字
    SOCKADDR_IN		ServerAddr;	// 服务端地址
    int			Port = 9999;	// 服务端端口号
    int 		Ret;		    // 用于判断 
    char		buf[1024];	    // 用来发送数据

    // 开始启动信息，2.2版本
    if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
    {
        printf("WSAStartup failed with error %d\n", Ret);
        return -1;
    }

    // 创建套接字，并判断是否成功
    if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        printf("socket failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    // 初始化服务端地址信息
    ServerAddr.sin_family = AF_INET;	// 使用ip4
    ServerAddr.sin_port = htons(Port);	// 字节转型，前文有介绍
    // 因为没有其它电脑，也没有安装虚拟机，所有只有使用本机的ip地址
    ServerAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");	// 将本机ip转为32位无符号整形

    // 提示信息
    printf("We are trying to connect to %s:%d...\n", inet_ntoa(ServerAddr.sin_addr), htons(ServerAddr.sin_port));

    // 建立链接（链接服务端)，并判断是否成功
    if (connect(s, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
    {
        printf("connect failed with error %d\n", WSAGetLastError());
        closesocket(s);	// 释放套接字
        WSACleanup();	// 清空启动信息
        return -1;
    }

    // 链接成功提示信息
    printf("Our connection succeeded.\n");
    printf("We will with hello send.\n");

    while (1)	// 可以一直发信息
    {
        //printf("The data you send is:\n");
        //scanf("%s", buf);	// 输入需要发送的数据

        Sleep(5000);
        // 数据发送处理 真正使用时，建议使用队列，可以保证数据的顺序
        sprintf(buf, "%s", "qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");

        // 发送数据，并判断是否失败，返回的是发送了多少数据
        if ((Ret = send(s, buf, strlen(buf), 0)) == SOCKET_ERROR)
        {
            printf("send failed with error %d\n", WSAGetLastError());
            closesocket(s);
            WSACleanup();
            return -1;
        }

        // 提示发送了多少数据
        printf("We successfully sent %d bytes.\n\n", Ret);
    }

    printf("We are closing the connection.\n");
    closesocket(s);
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
