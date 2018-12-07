#include"../Include.h"
#include"../Library/ExtensionMutex.h"
#include"../Library/Data.h"
#include"../Library/Cipher/OpenSSLAES.h"
#include"../Library/Cipher/OpenSSLDH.h"
#include"../Library/Cipher/OpenSSLRSA.h"
#include"../Library/Cipher/Cipher.h"
#include"CurlWrapper.h"
#include"Socket.h"
#include"Client.h"
#include "ClientController.h"


ClientController::ClientController()
{
}


ClientController::~ClientController()
{
	//�X���b�h�I������
	if(thread!=nullptr)thread->detach();
	thread = nullptr;

	//�������
	roomNumberList.clear();
	for (int i = 0; i < socketList.size(); i++) {
		socketList[i]=nullptr;
	}
	socketList.clear();
}

void ClientController::SetSocket(std::shared_ptr<Client> _socket)
{
	socketList.push_back(_socket);
}

bool ClientController::SerchNumber(int _number)
{
	for (auto element : socketList) {
		if (element->GetRoomNumber() == _number) {		
			return true;								//�������[���i���o�[����������
		}
	}
	return false;
}

void ClientController::StartThread(ClientController* _socketController)
{
	thread = std::make_shared<std::thread>(ControllerThreadLauncher, (void*)_socketController);
}


void ClientController::ControllerThread()
{
	while (1) {
		//�T�[�o�[�ɐڑ����Ă���N���C�A���g�����邩����
		if (socketList.empty() == true)continue;
		//���S�f�[�^�̏���
		try {
			for (auto& client : socketList) {
				if (client->EmptyCompleteData() == true)break;				//���S�f�[�^���Ȃ���Έȉ������͍s��Ȃ�
				DataManipulate(client.get(), client->GetCompleteData());
				MUTEX.Lock();
				client->DeleteCompleteData();
				MUTEX.Unlock();
			}
		}
		catch (std::exception error) {
			printf("ClientController=%s\n", error.what());
		}
		catch (...) {

		}

		//�ؒf�������s��ꂽ�\�P�b�g���폜
		int count = 0;
		for (int i = 0;i<socketList.size(); i++) {
			if (socketList[i]->GetState() == -1) {
				MUTEX.Lock();
				socketList[i]==nullptr;
				socketList.erase(socketList.begin() + i);
				MUTEX.Unlock();
			}
		}
	}
}


void ClientController::DataManipulate(Client* _socket, std::vector<char>* _data)
{
	char id = *(char*)&_data->at(0);
	switch (id) {
	case 0x01: {
		//���M�p�f�[�^
		char recvData[BYTESIZE];
		char encode[BYTESIZE];																		//�Í����f�[�^������
		char sendData[BYTESIZE];																	//���M�f�[�^

		int idsize = *(int*)&_data->at(sizeof(char));
		char* temp=(char*)malloc(idsize);
		memcpy(temp, &_data->at(sizeof(char) + sizeof(int)), idsize);
		std::shared_ptr<std::string> id = std::make_shared<std::string>(temp);
		MUTEX.Lock();
		_socket->GetData()->SetId(id);
		MUTEX.Unlock();

		//�v���C���[�̍��W�擾
		_socket->GetCurl()->DBGetPos(recvData,id);
		MUTEX.Lock();
		_socket->GetData()->SetX(*(float*)recvData);
		_socket->GetData()->SetY(*(float*)&recvData[sizeof(float)]);
		_socket->GetData()->SetZ(*(float*)&recvData[sizeof(float) * 2]);
		MUTEX.Unlock();

		//���M�f�[�^�쐬
		UserData userData;
		userData.data.size = sizeof(UserData) - sizeof(int);
		userData.data.id = 0x02;
		userData.x = _socket->GetData()->GetX();
		userData.y = _socket->GetData()->GetY();
		userData.z = _socket->GetData()->GetZ();


		//�Í�������
		char* origin = (char*)&userData;
		int encodeSize = _socket->GetAES()->Encode(encode, origin, sizeof(UserData));		//�Í���

		memcpy(sendData, &encodeSize, sizeof(int));
		memcpy(&sendData[sizeof(int)], encode, encodeSize);

		//���M����
		send(_socket->GetSocket(), sendData, sizeof(int) + encodeSize, 0);						//�쐬�����f�[�^�̍쐬

		_socket->StartHttpThread();

		//�������
		free(temp);
		id = nullptr;
		break;
	}
	//���W�X�V
	case 0x15: {
		//���M�p�f�[�^
		char encode[BYTESIZE];																		//�Í����f�[�^������
		char sendData[BYTESIZE];																	//���M�f�[�^

		//�f�[�^�̐��`�����l���Z�b�g
		MUTEX.Lock();
		_socket->GetData()->SetX(*(float*)&_data->at(sizeof(char)));
		_socket->GetData()->SetY(*(float*)&_data->at(sizeof(char) + sizeof(float) * 1));
		_socket->GetData()->SetZ(*(float*)&_data->at(sizeof(char) + sizeof(float) * 2));
		_socket->GetData()->SetAngle(*(float*)&_data->at(sizeof(char) + sizeof(float) * 3));
		_socket->GetData()->SetAnimation(*(int*)&_data->at(sizeof(char) + sizeof(float) * 4));
		MUTEX.Unlock();

		//���M�f�[�^�̍쐬
		PosData data;
		data.size = sizeof(PosData) - sizeof(int);
		data.id = 0x16;
		data.x = _socket->GetData()->GetX();
		data.y = _socket->GetData()->GetY();
		data.z = _socket->GetData()->GetZ();
		data.angle = _socket->GetData()->GetAngle();
		data.animation = _socket->GetData()->GetAnimation();

		//�Í�������
		char* origin = (char*)&data;
		MUTEX.Lock();
		int encodeSize = _socket->GetAES()->Encode(encode, origin, sizeof(PosData));		//�Í���
		MUTEX.Unlock();
		memcpy(sendData, &encodeSize, sizeof(int));
		memcpy(&sendData[sizeof(int)], encode, encodeSize);

		//���M����
		send(_socket->GetSocket(), sendData,sizeof(int)+encodeSize, 0);						//�쐬�����f�[�^�̍쐬
		//printf("���M�f�[�^\nsize=%d\nid=%04x\nx=%f\ny=%f\nz=%f\nangle=%f\nanimation=%d\n", data.size, data.id, data.x, data.y, data.z, data.angle, data.animation);
		break;
	}

	//�U������
	case 0x17:
		char encode[BYTESIZE];																				//�Í��f�[�^
		char sendData[BYTESIZE];																				//���M�f�[�^

		//���͎g���ĂȂ���������PC�ƂȂ����ꍇ�������g�p���邱�ƂɂȂ�
		int recvData = *(int*)&_data->at(sizeof(char));													//�N���U�����ꂽ�̂������̒��ɓ����Ă���

		//���M�f�[�^�̍쐬
		BaseData sendBuf;
		sendBuf.size = sizeof(BaseData) - sizeof(int);
		sendBuf.id = 0x18;

		//�Í�������
		int encodeSize = _socket->GetAES()->Encode(encode, (char*)&sendBuf, sizeof(BaseData));	//�Í�������
		memcpy(sendData, &encodeSize, sizeof(int));
		memcpy(&sendData[sizeof(int)], encode, encodeSize);
		
		//���M����
		send(_socket->GetSocket(), (char*)&sendData,encodeSize+sizeof(int), 0);				//�쐬�����f�[�^�̍쐬
		break;
	}
}




class A : public std::enable_shared_from_this<A>
{
	std::shared_ptr<A> GetMe() { return shared_from_this(); }
};