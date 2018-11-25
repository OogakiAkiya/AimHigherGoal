#include"../Include.h"
#include"../Library/ExtensionMutex.h"
#include"../Library/Data.h"
#include"../Library/Cipher/OpenSSLAES.h"
#include"../Library/Cipher/OpenSSLDH.h"
#include"../Library/Cipher/OpenSSLRSA.h"
#include"../Library/Cipher/Cipher.h"
#include"CurlWrapper.h"
#include"Socket.h"
#include "Client.h"

Client::Client()
{
	data = new Data();
	curl = new CurlWrapper();
	aes = new OpenSSLAES();
}

Client::~Client()
{
	//�������
	thread->join();											//�X���b�h�I��
	delete data;
	delete curl;
	delete thread;
	delete aes;
}

void Client::Recv()
{
	//������
	RecvLoop(EXCHANGEKEY);
	
	//���S�f�[�^�쐬����
	RecvLoop(CREATECOMPLETEDATA);
}

void Client::StartRecvThread(Client* _client)
{
	thread = new std::thread(RecvLauncher, (void*)_client);
}

void Client::StartHttpThread()
{
	curl->StartThread(curl, data);
}

SOCKET Client::GetSocket()
{
	return data->GetSocket();
}

int Client::GetRoomNumber()
{
	return roomNumber;
}

Data* Client::GetData()
{
	return data;
}

int Client::GetState()
{
	return state;
}

OpenSSLAES * Client::GetAES()
{
	return aes;
}


void Client::SetSocket(SOCKET _socket)
{
	data->SetSocket(_socket);
}

void Client::SetNumber(int _number)
{
	roomNumber = _number;
}

std::vector<char>* Client::GetCompleteData()
{
	return &completeDataQueList.front();
}

void Client::DeleteCompleteData()
{
	completeDataQueList.pop();
}

bool Client::EmptyCompleteData()
{
	if (completeDataQueList.empty() == true) {
		return true;									//��
	}
	return false;										//�l�������Ă���
}

void Client::RecvLoop(int _loopType)
{
	while (1) {
		int iResult;										//�����Ă����f�[�^�ʂ��i�[�����
		char rec[BYTESIZE];									//��M�f�[�^

		//��M
		iResult = recv(data->GetSocket(), rec, sizeof(rec), 0);
		if (iResult > 0) {
			//��M�f�[�^���ꎞ�f�[�^�z��ɒǉ�
			int nowSize = tempDataList.size();										//�ꎞ�f�[�^�z��ɉ�byte�f�[�^�������Ă��邩������
			tempDataList.resize(nowSize + iResult);									//�����Ă����f�[�^���i�[�ł���悤�Ɉꎞ�f�[�^�z��̃T�C�Y�ύX
			memcpy((char*)&tempDataList[nowSize], rec, iResult);					//�Ō���ɑ����Ă����f�[�^�̒ǉ�

			//������
			if (_loopType == EXCHANGEKEY) {
				if (ExchangeKey() == true)return;
			}
			//���S�f�[�^�쐬����
			if (_loopType == CREATECOMPLETEDATA)CreateCompleteData();

		}
		else if (iResult == 0) {
			//�ڑ����I������Ƃ�
			printf("�ؒf����܂���\n");
			state = -1;
			return;
		}
		else {
			//�ڑ��G���[���N��������
			printf("recv failed:%d\n%d", WSAGetLastError(), iResult);
			state = -1;
			return;
		}
	}
}

bool Client::ExchangeKey()
{
	while (tempDataList.size() >= sizeof(int)) {
		//�����ɏ\���ȃf�[�^�����邩�`�F�b�N
		int decodeSize = *(int*)&tempDataList[0];
		if (decodeSize <= (int)tempDataList.size() - sizeof(int)) {

			//�ϐ��錾
			char data[BYTESIZE];																			//�����O�f�[�^
			char decodeData[BYTESIZE];

			//��������
			memcpy(data, &tempDataList[sizeof(int)], decodeSize);											//��������f�[�^�̃R�s�[
			int outLen = CIPHER.GetOpenSSLRSA()->Decode(decodeData, data, decodeSize);						//���J���Í��̕���
			aes->SetKey((unsigned char*)decodeData, outLen);												//���ʌ���ݒ�
			tempDataList.erase(tempDataList.begin(), tempDataList.begin() + (decodeSize + sizeof(int)));	//���S�f�[�^�쐬�Ɏg�p���������폜
			return true;
		}
		else {
			return false;
		}
	}
}

void Client::CreateCompleteData()
{
	while (tempDataList.size() >= sizeof(int)) {
		//�����ɏ\���ȃf�[�^�����邩�`�F�b�N
		int decodeSize = *(int*)&tempDataList[0];
		if (decodeSize <= (int)tempDataList.size() - 4) {

			//�ϐ��錾
			char data[BYTESIZE];																			//�����O�f�[�^
			char decodeData[BYTESIZE];

			//��������
			memcpy(data, &tempDataList[sizeof(int)], decodeSize);
			aes->Decode(decodeData, data, decodeSize);

			//���S�f�[�^�̐���
			int byteSize = *(int*)decodeData;																//4byte�������擾��int�̒l�ɃL���X�g
			std::vector<char> compData(byteSize);															//���S�f�[�^
			memcpy(&compData[0], &decodeData[sizeof(int)], byteSize);										//�T�C�Y�ȊO�̃f�[�^���g�p�����S�f�[�^���쐬
			MUTEX.Lock();																					//�r������J�n
			completeDataQueList.push(compData);																//���S�f�[�^�z��Ɋi�[
			MUTEX.Unlock();																					//�r������I��
			tempDataList.erase(tempDataList.begin(), tempDataList.begin() + (decodeSize + sizeof(int)));	//���S�f�[�^�쐬�Ɏg�p���������폜
		}
		else {
			break;
		}
	}
}