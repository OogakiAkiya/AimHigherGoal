#include"../Include.h"
#include "Socket.h"

Socket::Socket()
{
	ZeroMemory(&hints, sizeof(hints));
	ip.erase();
	port.erase();
}

Socket::~Socket()
{
	Close();
}

Socket & Socket::SetProtocolVersion_IPv4()
{
	hints.ai_family = AF_INET;
	return *this;
}

Socket & Socket::SetProtocolVersion_IPv6()
{
	hints.ai_family = AF_INET6;
	return *this;
}

Socket & Socket::SetProtocolVersion_Dual()
{
	hints.ai_family = AF_UNSPEC;
	return *this;
}

Socket & Socket::SetProtocol_TCP()
{
	hints.ai_protocol = IPPROTO_TCP;		//TCP
	return *this;
}

Socket & Socket::SetProtocol_UDP()
{
	hints.ai_protocol = IPPROTO_UDP;		//UDP
	return *this;
}

Socket & Socket::SetIpAddress(std::string _addrs)
{
	ip=_addrs;
	return *this;
}

Socket & Socket::SetPortNumber(std::string _port)
{
	port = _port;
	return *this;
}

Socket * Socket::ServerCreate()
{
	AddressSet();
	Create();
	Bind();
	Listen();

	return this;
}

Socket * Socket::ClientCreate(bool _asynchronousFlag)
{
	AddressSet();
	Create();
	Connect();

	//非同期通信ON/OFF
	if (_asynchronousFlag) {
		unsigned long value = 1;
		ioctlsocket(socket, FIONBIO, &value);					//非同期通信化
	}

	return this;
}


void Socket::AddressSet()
{
	WSADATA wsaData;
	//ソケットが使用可能かのチェック
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		available = true;
		return;
	}

	//通信設定
	hints.ai_socktype = SOCK_STREAM;		//固定
	hints.ai_flags = AI_PASSIVE;
	iResult = getaddrinfo(NULL, port.c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed:%d\n", iResult);
		WSACleanup();
		available = true;
		return;
	}

}

void Socket::Create()
{
	//ソケットの作成
	socket = INVALID_SOCKET;
	socket = ::socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (socket == INVALID_SOCKET) {
		printf("Error at socket():%ld\n", WSAGetLastError());
		freeaddrinfo(result);					//メモリの解放
		WSACleanup();							//ソケットの解放
		available = true;
		return;
	}

}


void Socket::Connect()
{
	//接続
	int iResult = connect(socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(socket);
		socket = INVALID_SOCKET;
	}
	freeaddrinfo(result);
	if (socket == INVALID_SOCKET) {
		printf("Unable to server!\n");
		WSACleanup();
		return;
	}
	
}

int Socket::Recv(char* _recvbuf,int recvbuf_size,int flg)
{
	int byteSize = 0;
	byteSize=recv(socket, _recvbuf, recvbuf_size, 0);
	return byteSize;
}

void Socket::Close()
{
	int iResult = shutdown(socket, SD_SEND);						//今送っている情報を送りきって終わる
	if (iResult == SOCKET_ERROR) {
		closesocket(socket);
		WSACleanup();
	}
	closesocket(socket);
	WSACleanup();
}


void Socket::Bind()
{
	//bind
	int iResult = ::bind(socket, result->ai_addr, (int)result->ai_addrlen);				//IPアドレス(ローカルアドレスが入る)とポートの指定
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);																//メモリの解放
		closesocket(socket);																//ソケットのクローズ
		WSACleanup();																		//ソケットの解放
		return;
	}
}

void Socket::Listen()
{
	//listen
	if (listen(socket, SOMAXCONN) == SOCKET_ERROR) {										//バックログのサイズを設定
		printf("Listen failed with error:%ld\n", WSAGetLastError());
		closesocket(socket);
		WSACleanup();
		return;
	}

}

SOCKET Socket::Accept()
{
	SOCKET recvsocket;
	recvsocket = accept(socket, NULL, NULL);										//ここでクライアントがなければ待機
	if (recvsocket == INVALID_SOCKET) {
		printf("accept failed:%d\n", WSAGetLastError());
		closesocket(socket);
		WSACleanup();
		return 0;
	}
	return recvsocket;
}


SOCKET Socket::GetSocket()
{
	return socket;
}

Socket * Socket::Instantiate()
{
	return new Socket();
}
