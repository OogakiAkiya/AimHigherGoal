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
	//�ؒf���b�Z�[�W����
//�w�b�_�[�쐬
	Header header;
	header.size = sizeof(Header);
	header.playerIdSize = playerData->GetId()->length();
	memcpy(header.playerId, playerData->GetId()->c_str(), header.playerIdSize);
	header.id = 0xFE;
	//�f�[�^���M
	send(GetSocket(), (char*)&header, header.size, 0);

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
	ExchangeKey(*playerData->GetId());
	return true;
}

void Client::Recv()
{
	//���[�J���ϐ��쐬
	int iResult;																						//recv���ʂ�����(��M�����o�C�g��������)
	char rec[RECVBYTESIZE];																				//�󂯎�����f�[�^���i�[

	//��M
	while (1) {
		iResult = socket->Recv(rec, RECVBYTESIZE);
		if (iResult > 0) {

			//��M�f�[�^���ꎞ�f�[�^�z��ɒǉ�
			int nowSize = tempDataList.size();															//�ꎞ�f�[�^�z��ɉ�byte�f�[�^�������Ă��邩������
			tempDataList.resize(nowSize + iResult);														//�����Ă����f�[�^���i�[�ł���悤�Ɉꎞ�f�[�^�z��̃T�C�Y�ύX
			memcpy((char*)&tempDataList[nowSize], rec, iResult);										//�Ō���ɑ����Ă����f�[�^�̒ǉ�

			//�ꎞ�f�[�^���犮�S�f�[�^�̍쐬
			while (tempDataList.size() >= sizeof(int)) {												//��byte�̃f�[�^�������Ă��Ă����邩����ǂݍ��߂Ȃ���Δ�����
				//��������
				Header recvData = *(Header*)&tempDataList[0];
				if (recvData.size <= (int)tempDataList.size()) {
					int decodeSize = recvData.size - sizeof(Header);
					char data[BYTESIZE];																//�����O�f�[�^
					char decodeData[BYTESIZE];															//�����f�[�^
					memcpy(data, &tempDataList[sizeof(Header)], decodeSize);
					tempDataList.erase(tempDataList.begin(), tempDataList.begin() + recvData.size);		//���S�f�[�^�쐬�Ɏg�p���������폜
					mutex->Lock();
					int temp = cipher->GetOpenSSLAES()->Decode(decodeData, data, decodeSize);
					mutex->Unlock();
					DataProcess(recvData.id, decodeData);

				}
				else {
					//���S�f�[�^�쐬�s�\�ɂȂ����ꍇ
					break;
				}
			}

		}
		else if (iResult == 0) {
			//�f�[�^�������Ă��Ȃ������ꍇ�ؒf����
			break;
		}
		else {
			//�G���[����
			if (WSAGetLastError() != 10035) {									//�񓯊��̏ꍇ���̃G���[���Ăяo����邱�Ƃ��悭���邪�v���I�łȂ����߃X���[�����ėǂ�
				break;
			}
		}
	}
}

void Client::SendUserInformation(Data * _data)
{
	//�f�[�^����
	Header header;
	header.size = sizeof(Header);
	header.playerIdSize = _data->GetId()->length();
	memcpy(header.playerId, _data->GetId()->c_str(), header.playerIdSize);
	header.id = 0x01;
	send(GetSocket(), (char*)&header, sizeof(Header), 0);
}

void Client::SendPos(Data* _data)
{
	char sendData[BYTESIZE];
	char encodeData[BYTESIZE];

	//�f�[�^�쐬
	PosData data;
	data.x = _data->GetX();
	data.y = _data->GetY();
	data.z = _data->GetZ();
	data.angle = _data->GetAngle();
	data.animation = _data->GetAnimation();

	//�Í�����
	mutex->Lock();
	int encodeSize = cipher->GetOpenSSLAES()->Encode(encodeData, (char*)&data, sizeof(PosData));
	mutex->Unlock();

	//�w�b�_�[�쐬
	Header header;
	header.size = sizeof(Header) + encodeSize;
	header.playerIdSize = _data->GetId()->length();
	memcpy(header.playerId, _data->GetId()->c_str(), header.playerIdSize);
	header.id = 0x15;


	//���M�f�[�^�쐬
	memcpy(sendData, &header, sizeof(Header));
	memcpy(&sendData[sizeof(Header)], &encodeData, encodeSize);

	//�f�[�^���M
	send(GetSocket(), sendData, header.size, 0);

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

void Client::DataProcess(char _id, char * _data)
{
	Data data;

	switch (_id) {
	case 0x02: {
		Lock();
		playerData->SetX(*(float*)&_data[0]);
		playerData->SetY(*(float*)&_data[sizeof(float) * 1]);
		playerData->SetZ(*(float*)&_data[sizeof(float) * 2]);
		initFlag = true;
		Unlock();
		break;
	}
	 //���W�X�V����
	case 0x16: {

		float recvData = *(float*)&_data[0];
		data.SetX(recvData);
		recvData = *(float*)&_data[sizeof(float) * 1];
		data.SetY(recvData);
		recvData = *(float*)&_data[sizeof(float) * 2];
		data.SetZ(recvData);
		recvData = *(float*)&_data[sizeof(float) * 3];
		data.SetAngle(recvData);
		recvData = *(int*)&_data[sizeof(float) * 3 + sizeof(int)];
		data.SetAnimation(recvData);
		break;
	}
	//�U������
	case 0x18:
		data.SetX(0.0f);					//�G�̌��݂̍��W
		data.SetY(0.0f);
		data.SetZ(0.0f);
		data.SetAngle(0);
		data.SetAnimation(DAMAGE);
		break;
	}

}

void Client::ExchangeKey(std::string _id)
{
	char keyBuf[EVP_MAX_KEY_LENGTH];														//���T�C�Y
	char endata[BYTESIZE];
	char sendbuf[BYTESIZE];

	//�Í�����
	cipher->GetOpenSSLAES()->GetKey(keyBuf);												//���ʌ��擾
	mutex->Lock();
	int outlen = cipher->GetOpenSSLRSA()->Encode(endata, keyBuf, EVP_MAX_KEY_LENGTH);		//�Í�������
	mutex->Unlock();


	Header sendData;
	sendData.size = sizeof(Header) + outlen;
	sendData.playerIdSize = _id.size();
	memcpy(&sendData.playerId[0], _id.c_str(), sendData.playerIdSize);
	sendData.id = 0xFF;
	memcpy(sendbuf, (char*)&sendData, sizeof(Header));
	memcpy(&sendbuf[sizeof(Header)], (char*)&endata, outlen);

	//���M
	send(socket->GetSocket(), sendbuf, sendData.size, 0);
}

