#include"../Include.h"
#include"../Library/NamedPipe/NamedPipe.h"
#include"../Library/Data/Data.h"
#include"../Library/Cipher/OpenSSLAES.h"
#include"../Library/Cipher/OpenSSLRSA.h"
#include"../Library/Cipher/Cipher.h"
#include"../Library/CurlWrapper/CurlWrapper.h"
#include"../Library/Socket/Socket.h"
#include "Client.h"

Client::Client()
{
	tempDataQueue= std::make_unique<std::queue<NamedPipe::PipeData>>();			//�f�R�[�h�O�̃f�[�^
	completeDataQueue=std::make_unique<std::queue<NamedPipe::PipeData>>();		//���M�f�[�^

	completeDataQueList = std::make_unique <std::queue<std::vector<char>>>();
	data = std::make_shared<Data>();
	aes = std::make_unique<OpenSSLAES>();
}

Client::~Client()
{
	while (1) {
		if (tempDataQueue->empty())break;
		tempDataQueue->pop();
	}
	tempDataQueue = nullptr;			//�f�R�[�h�O�̃f�[�^
	while (1) {
		if (completeDataQueue->empty())break;
		completeDataQueue->pop();
	}
	completeDataQueue = nullptr;		//���M�f�[�^

	//�������
	aes=nullptr;
	completeDataQueList = nullptr;
	data = nullptr;

}

void Client::Update()
{
	DecryptionManipulate();
	/*
	//������
	if (!keyChangeFlg) {
		RecvLoop(EXCHANGEKEY);
	}
	//���S�f�[�^�쐬����
	if (keyChangeFlg) {
		RecvLoop(CREATECOMPLETEDATA);
	}
	*/
}


SOCKET Client::GetSocket()
{
	return data->GetSocket();
}

std::shared_ptr<Data> Client::GetData()
{
	return data;
}

int Client::GetState()
{
	return state;
}

OpenSSLAES* Client::GetAES()
{
	return aes.get();
}

void Client::SetPosGetFlg()
{
	posGetFlg = true;
}

bool Client::GetPosGetFlg()
{
	return posGetFlg;
}

std::vector<char>* Client::GetCompleteData()
{
	return &completeDataQueList->front();
}

void Client::DeleteCompleteData()
{
	completeDataQueList->pop();
}

bool Client::EmptyCompleteData()
{
	if (completeDataQueList->empty() == true) {
		return true;									//��
	}
	return false;										//�l�������Ă���
}

void Client::AddData(NamedPipe::PipeData* _data)
{
	tempDataQueue->push(*_data);
}

void Client::RecvLoop(int _loopType)
{
		int iResult;										//�����Ă����f�[�^�ʂ��i�[�����
		char rec[BYTESIZE*2];								//��M�f�[�^

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
		else if (WSAGetLastError() == 10035) {
			//�񓯊��̏ꍇclient��send���Ă��Ȃ������Ƃ��ɂ�����G���[
			return;
		}
		else {
			//�ڑ��G���[���N��������
			printf("recv failed:%d\n%d", WSAGetLastError(), iResult);
			state = -1;
			return;
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
			keyChangeFlg = true;
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
		if (decodeSize <= (int)tempDataList.size() - sizeof(int)) {

			//�ϐ��錾
			char data[BYTESIZE];																			//�����O�f�[�^
			char decodeData[BYTESIZE];
			//��������
			memcpy(data, &tempDataList[sizeof(int)], decodeSize);
			aes->Decode(decodeData, data, decodeSize);

			//���S�f�[�^�̐���
			int byteSize = *(int*)decodeData;																	//4byte�������擾��int�̒l�ɃL���X�g
			if (byteSize < BYTESIZE&&byteSize>0) {
				std::vector<char> compData(byteSize);															//���S�f�[�^
				memcpy(&compData[0], &decodeData[sizeof(int)], byteSize);										//�T�C�Y�ȊO�̃f�[�^���g�p�����S�f�[�^���쐬
				completeDataQueList->push(compData);																//���S�f�[�^�z��Ɋi�[
			}
			tempDataList.erase(tempDataList.begin(), tempDataList.begin() + (decodeSize + sizeof(int)));		//���S�f�[�^�쐬�Ɏg�p���������폜

		}
		else {
			break;
		}
	}
}

void Client::DecryptionManipulate()
{
	while (1) {
		if (tempDataQueue->empty())break;
		NamedPipe::PipeData sendData;
		char data[BYTESIZE];																			//�����O�f�[�^
		char decodeData[BYTESIZE];

		if (!keyChangeFlg) {
			int idSize = *(int*)&tempDataQueue->front().data[sizeof(int)];
			int decodeSize = tempDataQueue->front().byteSize - (sizeof(int) * 2 + idSize);

			int outLen = CIPHER.GetOpenSSLRSA()->Decode(decodeData, &tempDataQueue->front().data[sizeof(int) * 2 + idSize], decodeSize);						//���J���Í��̕���
			aes->SetKey((unsigned char*)decodeData, outLen);												//���ʌ���ݒ�
			tempDataQueue->pop();
			keyChangeFlg = true;
		}

		//�ȉ���������
	}
}
