#include"../StandardLibraryInclude.h"
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

Socket * Socket::ClientCreate(bool _asynchronousflg)
{
	AddressSet();
	Create();
	Connect();
	//�񓯊��ʐMON/OFF
	if (_asynchronousflg) {
		unsigned long value = 1;
		ioctlsocket(m_socket, FIONBIO, &value);					//�񓯊��ʐM��
	}

	return this;
}


void Socket::AddressSet()
{
	WSADATA wsaData;
	//socket�g�p�\���̃`�F�b�N
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		is_available = true;
		return;
	}

	//�ʐM�ݒ�
	hints.ai_socktype = SOCK_STREAM;		//�Œ�
	hints.ai_flags = AI_PASSIVE;
	iResult = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed:%d\n", iResult);
		WSACleanup();
		is_available = true;
		return;
	}

}

void Socket::Create()
{
	//�\�P�b�g�̍쐬
	m_socket = INVALID_SOCKET;
	m_socket = ::socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (m_socket == INVALID_SOCKET) {
		printf("Error at socket():%ld\n", WSAGetLastError());
		freeaddrinfo(result);					//�������̉��
		WSACleanup();							//�\�P�b�g�̉��
		is_available = true;
		return;
	}

}


void Socket::Connect()
{
	//�ڑ�
	int iResult = connect(m_socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
	freeaddrinfo(result);
	if (m_socket == INVALID_SOCKET) {
		printf("Unable to server!\n");
		WSACleanup();
		return;
	}
	
}

int Socket::Recv(char* _recvbuf,int recvbuf_size,int flg)
{
	int bytesize = 0;
	bytesize=recv(m_socket, _recvbuf, recvbuf_size, 0);
	return bytesize;
}

void Socket::Close()
{
	int iResult = shutdown(m_socket, SD_SEND);						//�������Ă�����𑗂肫���ďI���
	if (iResult == SOCKET_ERROR) {
		closesocket(m_socket);
		WSACleanup();
	}
	closesocket(m_socket);
	WSACleanup();
}


void Socket::Bind()
{
	//bind
	int iResult = ::bind(m_socket, result->ai_addr, (int)result->ai_addrlen);				//IP�A�h���X(���[�J���A�h���X������)�ƃ|�[�g�̎w��
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);																//�������̉��
		closesocket(m_socket);																//�\�P�b�g�̃N���[�Y
		WSACleanup();																		//�\�P�b�g�̉��
		return;
	}
}

void Socket::Listen()
{
	//listen
	if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR) {										//�o�b�N���O�̃T�C�Y��ݒ�
		printf("Listen failed with error:%ld\n", WSAGetLastError());
		closesocket(m_socket);
		WSACleanup();
		return;
	}

}

SOCKET Socket::Accept()
{
	SOCKET recvsocket;

	//accept����
	recvsocket = accept(m_socket, NULL, NULL);										//�����ŃN���C�A���g���Ȃ���Αҋ@
	if (recvsocket == INVALID_SOCKET) {
		printf("accept failed:%d\n", WSAGetLastError());
		closesocket(m_socket);
		WSACleanup();
		return 0;
	}
	return recvsocket;
}


SOCKET Socket::GetSocket()
{
	return m_socket;
}

Socket * Socket::Instantiate()
{
	return new Socket();
}
