#include"../StandardLibraryInclude.h"
#include"Socket.h"
#include"ExtensionMutex.h"
#include"Data.h"
#include"Cipher/OpenSSLAES.h"
#include"Cipher/OpenSSLRSA.h"
#include"Cipher/Cipher.h"
#include"Client.h"

#pragma comment(lib,"Ws2_32.lib")
using namespace std;

Client::Client()
{
	//�C���X�^���X�̐���
	cipher = new Cipher();										//�Í������p�N���X
	mutex = new ExtensionMutex();								//�r������p�N���X
	playerData = new Data();
}

Client::~Client()
{
	//�������
	if (socket != nullptr)socket->Close();
	if (thread != nullptr)thread->detach();
	thread = nullptr;
	delete mutex;
	delete cipher;
	delete socket;
	delete playerData;
}


void Client::StartThread()
{
	//thread = new std::thread(ClientThreadLauncher, (void*)this);
	thread = std::make_shared<std::thread>(ClientThreadLauncher,this);
}

bool Client::CreateSocket(std::string _ip)
{
	//���Ƀ\�P�b�g�����݂���ꍇ�폜����
	if (socket != nullptr) {
		socket->Close();
		delete socket;
		socket = nullptr;
	}

	//�\�P�b�g����
	socket = Socket::Instantiate()->
		SetProtocolVersion_Dual().
		SetProtocol_TCP().
		SetIpAddress(_ip.c_str()).
		SetPortNumber("49155").
		ClientCreate();
	if (socket == nullptr)return false;
	playerData->SetSocket(socket->GetSocket());
	//Recv�������s���X���b�h���J�n������
	StartThread();

	//����������
	ExchangeKey();
	return true;
}

void Client::Recv()
{
	//���[�J���ϐ��쐬
	int iResult;																							//recv���ʂ�����(��M�����o�C�g��������)
	char rec[BYTESIZE * 2];																					//�󂯎�����f�[�^���i�[

	//��M
	while (1) {
		iResult = socket->Recv(rec, BYTESIZE);
		if (iResult > 0) {

			//��M�f�[�^���ꎞ�f�[�^�z��ɒǉ�
			int nowSize = tempDataList.size();																//�ꎞ�f�[�^�z��ɉ�byte�f�[�^�������Ă��邩������
			tempDataList.resize(nowSize + iResult);															//�����Ă����f�[�^���i�[�ł���悤�Ɉꎞ�f�[�^�z��̃T�C�Y�ύX
			memcpy((char*)&tempDataList[nowSize], rec, iResult);											//�Ō���ɑ����Ă����f�[�^�̒ǉ�

			//�ꎞ�f�[�^���犮�S�f�[�^�̍쐬
			while (tempDataList.size() >= sizeof(int)) {													//��byte�̃f�[�^�������Ă��Ă����邩����ǂݍ��߂Ȃ���Δ�����
				try {
					//��������
					int decodeSize = *(int*)&tempDataList[0];
					if (decodeSize <= (int)tempDataList.size() - sizeof(int)) {
						char data[BYTESIZE];																	//�����O�f�[�^
						char decodeData[BYTESIZE];																//�����f�[�^
						memcpy(data, &tempDataList[sizeof(int)], decodeSize);
						cipher->GetOpenSSLAES()->Decode(decodeData, data, decodeSize);
						int byteSize = *(int*)decodeData;														//4byte�������擾��int�̒l�ɃL���X�g
						std::vector<char> compData(byteSize);													//���S�f�[�^
						memcpy(&compData[0], &decodeData[sizeof(int)], byteSize);								//�T�C�Y�ȊO�̃f�[�^���g�p�����S�f�[�^���쐬
						tempDataList.erase(tempDataList.begin(), tempDataList.begin() + (decodeSize + 4));		//���S�f�[�^�쐬�Ɏg�p���������폜

						//���S�f�[�^�̏���
						DataManipulate(&compData);
					}

					else {
						//���S�f�[�^�쐬�s�\�ɂȂ����ꍇ
						break;
					}
				}
				catch (std::exception error) {
					printf("%s", error.what());
				}
			}
		}
		else if (iResult == 0) {
			//�f�[�^�������Ă��Ȃ������ꍇ�ؒf����
			break;
		}
		else {
			//�G���[����
			int error = WSAGetLastError();
			if (error != 10035) {									//�񓯊��̏ꍇ���̃G���[���Ăяo����邱�Ƃ��悭���邪�v���I�łȂ����߃X���[�����ėǂ�
				break;
			}
		}
	}
}

void Client::SendUserInformation(Data * _data)
{
	//�f�[�^����
	UserData sendData;
	sendData.base.id = 0x01;
	sendData.idsize = _data->GetId()->length();
	memcpy(&sendData.id[0], _data->GetId()->c_str(), sendData.idsize);
	sendData.base.size = sizeof(UserData) - sizeof(int);

	char* origin = (char*)&sendData;
	char encodeData[BYTESIZE];								//�Í����f�[�^������
	char senData[BYTESIZE];									//���M�f�[�^

	//�Í�����
	int encode_size = cipher->GetOpenSSLAES()->Encode(encodeData, origin,sendData.base.size+sizeof(UserData));
	memcpy(senData, &encode_size, sizeof(int));
	memcpy(&senData[sizeof(int)], encodeData, encode_size);

	//�f�[�^���M
	send(socket->GetSocket() , senData, sizeof(int) + encode_size, 0);

}

void Client::SendPos(Data* _data)
{
	//���M�f�[�^�̐���
	PosData data;
	data.base.size = sizeof(PosData) - sizeof(int);
	data.base.id = 0x15;
	data.x = _data->GetX();
	data.y = _data->GetY();
	data.z = _data->GetZ();
	data.angle = _data->GetAngle();
	data.animation = _data->GetAnimation();

	//�ϐ�����
	char* origin = (char*)&data;
	char encodeData[BYTESIZE];										//�Í����f�[�^������
	char sendData[BYTESIZE];										//���M�f�[�^

	//�Í�����
	int encode_size = cipher->GetOpenSSLAES()->Encode(encodeData, origin, sizeof(PosData));
	memcpy(sendData, &encode_size, sizeof(int));
	memcpy(&sendData[sizeof(int)], encodeData, encode_size);

	//�f�[�^���M
	send(socket->GetSocket(), sendData, sizeof(int) + encode_size, 0);
}

void Client::SendAttack(Data* _data)
{
	//�����������ǂ����𔻒�
	float length;
	D3DXVECTOR3 vectorLength;
	D3DXVECTOR3 playerVector(_data->GetX(), _data->GetY(), _data->GetZ());											//�v���C���[�̃x�N�g��
	for (int element = 0; element < 3; element++) {																	//id�͓G�̋��
		D3DXVECTOR3 enemyVector(enemyData[element].GetX(), enemyData[element].GetY(), enemyData[element].GetZ());	//�G�̃x�N�g��
		vectorLength = enemyVector - playerVector;																	//��̃x�N�g���̍�
		length = D3DXVec3Length(&vectorLength);

		//�f�[�^���M(���������ꍇ)
		if (length <= 2.0f) {
			char encodeData[BYTESIZE];					//�Í��f�[�^
			char sendData[BYTESIZE];					//���M�f�[�^

			//�f�[�^�̐���
			AttckData data;
			data.base.size = sizeof(AttckData) - sizeof(int);
			data.base.id = 0x17;
			data.socket = element;
			
			//�Í�����
			int encodeSize = cipher->GetOpenSSLAES()->Encode(encodeData, (char*)&data, sizeof(AttckData));
			memcpy(sendData, &encodeSize, sizeof(int));
			memcpy(&sendData[sizeof(int)], encodeData, encodeSize);

			//���M����
			send(socket->GetSocket(), (char*)&sendData, sizeof(int) + encodeSize, 0);
		}
	}
}

float Client::GetCount()
{
	return count;
}

SOCKET Client::GetSocket()
{
	return socket->GetSocket();
}

Data Client::GetData()
{
	return dataQueueList.front();
}

Data* Client::GetPlayerData()
{
	return playerData;
}

bool Client::GetInitFlg()
{
	return initFlag;
}

void Client::DeleteData()
{
	dataQueueList.pop();
}

void Client::ClearData()
{
	while (1) {
		DeleteData();
		if (DataEmpty() == true)break;
	}
}

bool Client::DataEmpty()
{
	if (dataQueueList.empty() == true)return true;
	return false;
}

void Client::SetEnemyData(int _enemyid, Data * data)
{
	enemyData[_enemyid] = *data;
}

void Client::Lock()
{
	mutex->Lock();
}

void Client::Unlock()
{
	mutex->Unlock();
}

void Client::DataManipulate(const std::vector<char>* _data)
{
	Data data;
	char id = *(char*)&_data->at(0);					//�Ȃ�̃f�[�^��id�Ŕ��f����

	switch (id) {
	case 0x02: {
		Lock();
		playerData->SetX(*(float*)&_data->at(sizeof(char)));
		playerData->SetY(*(float*)&_data->at(sizeof(char) + sizeof(float) * 1));
		playerData->SetZ(*(float*)&_data->at(sizeof(char) + sizeof(float) * 2));
		initFlag = true;
		Unlock();
		break;
	}

	//���W�X�V����
	case 0x16: {
		float recvData = *(float*)&_data->at(sizeof(char) + sizeof(float) * 0);
		data.SetX(recvData);
		recvData = *(float*)&_data->at(sizeof(char) + sizeof(float) * 1);
		data.SetY(recvData);
		recvData = *(float*)&_data->at(sizeof(char) + sizeof(float) * 2);
		data.SetZ(recvData);
		recvData = *(float*)&_data->at(sizeof(char) + sizeof(float) * 3);
		data.SetAngle(recvData);
		recvData = *(int*)&_data->at(sizeof(char) + sizeof(float) * 3 + sizeof(int));
		data.SetAnimation(recvData);
		Lock();
		dataQueueList.push(data);
		Unlock();
		break;
	}

	//�U������
	case 0x18:
		data.SetX(0.0f);					//�G�̌��݂̍��W
		data.SetY(0.0f);
		data.SetZ(0.0f);
		data.SetAngle(0);
		data.SetAnimation(DAMAGE);
		Lock();
		dataQueueList.push(data);
		Unlock();
		break;
	}

}

void Client::ExchangeKey()
{
	//�ϐ�����
	char keyBuf[EVP_MAX_KEY_LENGTH];							//���T�C�Y
	char endata[BYTESIZE];
	char sendbuf[BYTESIZE];

	//�Í�����
	cipher->GetOpenSSLAES()->GetKey(keyBuf);												//���ʌ��擾
	int outlen = cipher->GetOpenSSLRSA()->Encode(endata, keyBuf, EVP_MAX_KEY_LENGTH);		//�Í�������
	memcpy(sendbuf, &outlen, sizeof(int));
	memcpy(&sendbuf[sizeof(int)], &endata, outlen);
	
	//���M
	send(socket->GetSocket(), sendbuf, sizeof(int) + outlen, 0);

}

