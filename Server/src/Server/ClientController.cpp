#include"../Include.h"
#include"../Library/Mutex/ExtensionMutex.h"
#include"../Library/NamedPipe/NamedPipe.h"
#include"../Library/Data/Data.h"
#include"../Library/Cipher/OpenSSLAES.h"
#include"../Library/Cipher/OpenSSLRSA.h"
#include"../Library/Cipher/Cipher.h"
#include"../Library/CurlWrapper/CurlWrapper.h"
#include"../Library/Socket/Socket.h"
#include"Client.h"
#include "ClientController.h"


ClientController::ClientController()
{
}


ClientController::~ClientController()
{
	//�������
	roomNumberList.clear();
	for (int i = 0; i < clientList.size(); i++) {
		clientList[i]=nullptr;
	}
	clientList.clear();
}

void PosRegistration(std::string _data)
{
	std::unique_ptr<CurlWrapper> curl = std::make_unique<CurlWrapper>();
	curl->HTTPConnect(nullptr, "http://lifestyle-qa.com/update_user_arraydata.php", _data,"DBCreateData");
	curl = nullptr;
}

void ClientController::Update()
{
	ControllerUpdate();
	SocketUpdate();
	DBCreateData();
}

void ClientController::AddClient(std::shared_ptr<Client> _client)
{
	clientList.push_back(_client);
}


void ClientController::ControllerUpdate()
{
	//�T�[�o�[�ɐڑ����Ă���N���C�A���g�����邩����
	if (clientList.empty() == true)return;
	//���S�f�[�^�̏���
	for (auto& client : clientList) {
		if (client->EmptyCompleteData() == true)break;				//���S�f�[�^���Ȃ���Έȉ������͍s��Ȃ�
		DataManipulate(client.get(), client->GetCompleteData());
		client->DeleteCompleteData();
	}

	//�ؒf�������s��ꂽ�\�P�b�g���폜
	int count = 0;
	for (int i = 0; i < clientList.size(); i++) {
		if (clientList[i]->GetState() == -1) {
			clientList[i] == nullptr;
			clientList.erase(clientList.begin() + i);
		}
	}
}

void ClientController::SocketUpdate()
{
	if (clientList.empty() == true)return;
	for (auto& client : clientList) {
		client->Update();
	}
}

void ClientController::DataManipulate(Client* _socket, std::vector<char>* _data)
{
	char id = *(char*)&_data->at(0);
	switch (id) {
	case 0x01: {
		//���M�p�f�[�^
		char recvData[BYTESIZE];
		char sendData[BYTESIZE];																	//���M�f�[�^

		//�v���C���[ID�̐��`
		int idsize = *(int*)&_data->at(sizeof(char));
		char* temp=(char*)malloc(idsize);
		memcpy(temp, &_data->at(sizeof(char) + sizeof(int)), idsize);
		std::shared_ptr<std::string> playerId = std::make_shared<std::string>(temp);
		playerId->resize(idsize);																	//id�̃T�C�Y�����ɂȂ�Ȃ��̂�resize

		_socket->GetData()->SetId(playerId);

		//�v���C���[�̍��W�擾
		DBGetPos(recvData, playerId);

		//�擾�����f�[�^���Z�b�g
		_socket->GetData()->SetX(*(float*)recvData);
		_socket->GetData()->SetY(*(float*)&recvData[sizeof(float)]);
		_socket->GetData()->SetZ(*(float*)&recvData[sizeof(float) * 2]);

		//���M�f�[�^�쐬
		UserData userData;
		userData.data.size = sizeof(UserData) - sizeof(int);
		userData.data.id = 0x02;
		userData.x = _socket->GetData()->GetX();
		userData.y = _socket->GetData()->GetY();
		userData.z = _socket->GetData()->GetZ();


		//���M�p�f�[�^�̍쐬
		int amountSize=CreateSendData(sendData, _socket, (char*)&userData, sizeof(UserData));

		//���M����
		send(_socket->GetSocket(), sendData, amountSize, 0);

		//�������
		free(temp);
		playerId = nullptr;
		_socket->SetPosGetFlg();
		break;
	}
	//���W�X�V
	case 0x15: {
		//���M�p�f�[�^
		char sendData[BYTESIZE];																	//���M�f�[�^

		//�f�[�^�̐��`�����l���Z�b�g
		_socket->GetData()->SetX(*(float*)&_data->at(sizeof(char)));
		_socket->GetData()->SetY(*(float*)&_data->at(sizeof(char) + sizeof(float) * 1));
		_socket->GetData()->SetZ(*(float*)&_data->at(sizeof(char) + sizeof(float) * 2));
		_socket->GetData()->SetAngle(*(float*)&_data->at(sizeof(char) + sizeof(float) * 3));
		_socket->GetData()->SetAnimation(*(int*)&_data->at(sizeof(char) + sizeof(float) * 4));

		//���M�f�[�^�̍쐬
		PosData data;
		data.size = sizeof(PosData) - sizeof(int);
		data.id = 0x16;
		data.x = _socket->GetData()->GetX();
		data.y = _socket->GetData()->GetY();
		data.z = _socket->GetData()->GetZ();
		data.angle = _socket->GetData()->GetAngle();
		data.animation = _socket->GetData()->GetAnimation();

		//���M�p�f�[�^�̍쐬
		int amountSize = CreateSendData(sendData, _socket, (char*)&data, sizeof(PosData));

		//���M����
		send(_socket->GetSocket(), sendData, amountSize , 0);
		//printf("���M�f�[�^\nsize=%d\nid=%04x\nx=%f\ny=%f\nz=%f\nangle=%f\nanimation=%d\n", data.size, data.id, data.x, data.y, data.z, data.angle, data.animation);
		break;
	}

	//�U������
	case 0x17:
		char sendData[BYTESIZE];																				//���M�f�[�^

		//���͎g���ĂȂ���������PC�ƂȂ����ꍇ�������g�p���邱�ƂɂȂ�
		int recvData = *(int*)&_data->at(sizeof(char));													//�N���U�����ꂽ�̂������̒��ɓ����Ă���

		//���M�f�[�^�̍쐬
		BaseData sendBuf;
		sendBuf.size = sizeof(BaseData) - sizeof(int);
		sendBuf.id = 0x18;

		//���M�p�f�[�^�̍쐬
		int amountSize = CreateSendData(sendData, _socket, (char*)&sendBuf, sizeof(BaseData));

		//���M����
		send(_socket->GetSocket(), (char*)&sendData,amountSize, 0);
		break;
	}
}

void ClientController::DBCreateData()
{
	std::stringstream query;						//web�T�[�o�[�ɑ���f�[�^
	std::string output;								//query�̂܂܂��ƃG���[���N����string�^�ɓ����ƂȂ��Ȃ�
	std::vector<std::shared_ptr<Data>> datas;		//���M����f�[�^�ꗗ

	//����f�[�^�̑I��
	for (auto socket : clientList) {
		if (socket->GetPosGetFlg()) {
			datas.push_back(socket->GetData());
		}
	}
	if (datas.size() <= 0)return;

	//�f�[�^�̍쐬
	query << "amount=" << datas.size();
	for (int i = 0; i < datas.size(); i++) {
		query << "&" << "player" << i << "=" << datas[i]->GetId()->c_str();
		query << "&" << "x" << i << "=" << datas[i]->GetX();
		query << "&" << "y" << i << "=" << datas[i]->GetY();
		query << "&" << "z" << i << "=" << datas[i]->GetZ();
	}
	query >> output;

	std::thread thread(PosRegistration, output);
	thread.detach();


}

void ClientController::DBGetPos(char * _data, std::shared_ptr<std::string> _userId)
{
	//���[�U�[�ǉ�����
	std::stringstream query;
	std::string output = "";																//���M�p�f�[�^
	std::string buf;																		//�󂯎�����f�[�^���i�[����
	std::string error;
	char recvdata[256];

	//���M�f�[�^�̐���
	query << "player=" << _userId.get()->c_str();
	query >> output;

	std::unique_ptr<CurlWrapper> curl = std::make_unique<CurlWrapper>();
	curl->HTTPConnect(&buf, "http://lifestyle-qa.com/get_pos.php", output.c_str(),*_userId.get());
	curl = nullptr;

	//json����l�̎擾
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	if (buf.length() != 0) {
		auto json = json11::Json::parse(buf, error);

		x = std::stof(json["x"].string_value());
		y = std::stof(json["y"].string_value());
		z = std::stof(json["z"].string_value());
	}

	//�f�[�^���
	memcpy(recvdata, &x, sizeof(float));
	memcpy(&recvdata[sizeof(float)], &y, sizeof(float));
	memcpy(&recvdata[sizeof(float) * 2], &z, sizeof(float));
	memcpy(_data, recvdata, sizeof(float) * 3);

}


int ClientController::CreateSendData(char* _encryptionData, Client* _socket,char* _originalData,int _dataLen)
{
	char encode[BYTESIZE];																		//�Í����f�[�^

	int encodeSize = _socket->GetAES()->Encode(encode, _originalData, _dataLen);				//�Í���
	memcpy(_encryptionData, &encodeSize, sizeof(int));											//���̃f�[�^�T�C�Y���擪�ɓ���
	memcpy(&_encryptionData[sizeof(int)], encode, encodeSize);									//�Í����f�[�^������

	return encodeSize+sizeof(int);																//�Í����̃f�[�^�T�C�Y+�擪�ɓ��ꂽ�f�[�^�T�C�Y
}

