#ifndef Socket_h
#define Socket_h

class Socket{
public:
	Socket();
	~Socket();

	//---------------------------------------------------------
	//�C���X�^���X�𐶐�����^�C�~���O�ŒʐM���@�Ȃǂ�ݒ肷��
	//*�`Create���Ō�ɌĂ�*
	//---------------------------------------------------------
	Socket & SetProtocolVersion_IPv4();										//IPv4
	Socket & SetProtocolVersion_IPv6();										//IPv6
	Socket & SetProtocolVersion_Dual();										//IPv4��IPv6�̗����ƒʐM�\(�N���C�A���g�ł̂ݎg�p�\)
	Socket & SetProtocol_TCP();												//TCP
	Socket & SetProtocol_UDP();												//UDP
	Socket & SetIpAddress(std::string _addrs);								//IP�A�h���X�̐ݒ�
	Socket & SetPortNumber(std::string _port);								//�|�[�g�ԍ��̐ݒ�
	Socket & SetAsynchronous();												//�񓯊��ݒ�
	std::shared_ptr<Socket> ServerCreate(std::shared_ptr<Socket> _this);	//�T�[�o�[�̐���
	std::shared_ptr<Socket> ClientCreate(std::shared_ptr<Socket> _this);	//�N���C�A���g�̐���

	//---------------------------------------------------------
	//�ʐM��e�������s��
	//---------------------------------------------------------
	SOCKET Accept();														//�ڑ��ɗ����\�P�b�g���擾(�T�[�o�[�Ŏg�p)
	int Recv(char* _recvbuf, int recvbuf_size, int flg = 0);				//�f�[�^��M(�T�[�o�[�Ŏg�p)
	void Close();															//�ʐM��ؒf����
	SOCKET GetSocket();														//�\�P�b�g��Ԃ�


private:
	//---------------------------------------------------------
	//���ʂŎg�p
	//---------------------------------------------------------
	bool AddressSet();
	bool Create();

	//---------------------------------------------------------
	//�T�[�o�[�Ŏg�p
	//---------------------------------------------------------
	bool Bind();
	bool Listen();

	//---------------------------------------------------------
	//�N���C�A���g�Ŏg�p
	//---------------------------------------------------------
	bool Connect();

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::unique_ptr<std::string> ip;										//IP�A�h���X
	std::unique_ptr<std::string> port;										//�|�[�g�ԍ�
	SOCKET socket;															//�\�P�b�g
	bool asynchronousFlg = false;											//�񓯊��t���O
	struct addrinfo *result = NULL, hints;									//�\�P�b�g�ݒ�p�ϐ�

};

#endif
