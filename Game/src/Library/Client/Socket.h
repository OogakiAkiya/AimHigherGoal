class Socket {
public:
	Socket();
	~Socket();

	//---------------------------------------------------------
	//�C���X�^���X�𐶐�����^�C�~���O�ŒʐM���@�Ȃǂ�ݒ肷��
	//*�`Create���Ō�ɌĂ�*
	//---------------------------------------------------------
	Socket & SetProtocolVersion_IPv4();							//IPv4
	Socket & SetProtocolVersion_IPv6();							//IPv6
	Socket & SetProtocolVersion_Dual();							//IPv4��IPv6�̗����ƒʐM�\(�N���C�A���g�ł̂ݎg�p�\)
	Socket & SetProtocol_TCP();									//TCP
	Socket & SetProtocol_UDP();									//UDP
	Socket & SetIpAddress(std::string addrs);					//IP�A�h���X�̐ݒ�
	Socket & SetPortNumber(std::string _port);					//�|�[�g�ԍ��̐ݒ�
	Socket* ServerCreate();										//�T�[�o�[�̐���
	Socket* ClientCreate(bool _asynchronousflg=false);										//�N���C�A���g�̐���

	//---------------------------------------------------------
	//�ʐM��e�������s��
	//---------------------------------------------------------
	SOCKET Accept();											//�ڑ��ɗ����\�P�b�g���擾(�T�[�o�[�Ŏg�p)
	int Recv(char* _recvbuf, int recvbuf_size, int flg = 0);	//�f�[�^��M(�T�[�o�[�Ŏg�p)
	void Close();												//�ʐM��ؒf����
	SOCKET GetSocket();											//�\�P�b�g��Ԃ�
	static Socket* Instantiate();

private:
	//---------------------------------------------------------
	//���ʂŎg�p
	//---------------------------------------------------------
	void AddressSet();
	void Create();

	//---------------------------------------------------------
	//�T�[�o�[�Ŏg�p
	//---------------------------------------------------------
	void Bind();
	void Listen();

	//---------------------------------------------------------
	//�N���C�A���g�Ŏg�p
	//---------------------------------------------------------
	void Connect();

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::string ip="localhost";									//IP�A�h���X
	std::string port="49155";									//�|�[�g�ԍ�
	SOCKET m_socket;											//�\�P�b�g
	bool is_available = false;
	struct addrinfo *result = NULL, hints;

};
