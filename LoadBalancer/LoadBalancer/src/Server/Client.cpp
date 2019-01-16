#include"../Include.h"
#include"../Library/Data/Data.h"
#include"../Library/NamedPipe/NamedPipe.h"
#include"../Library/Socket/Socket.h"
#include "Client.h"

Client::Client()
{
	pipeDataList = std::make_shared<std::queue<NamedPipe::PipeData>>();
	data = std::make_shared<Data>();
}

Client::~Client()
{
	//�������
	pipeDataList = nullptr;
	data = nullptr;

}

void Client::Update()
{
	RecvLoop();
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

NamedPipe::PipeData* Client::GetPipeData()
{
	return &pipeDataList->front();
}

void Client::DeletePipeData()
{
	pipeDataList->pop();
}

bool Client::EmptyPipeData()
{
	if (pipeDataList->empty() == true) {
		return true;									//��
	}
	return false;										//�l�������Ă���
}

void Client::RecvLoop()
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

			while (tempDataList.size() >= sizeof(int)) {
				//�p�C�v�ɑ���f�[�^�����邩�`�F�b�N
				int oneDataSize = *(int*)&tempDataList[0];
				if (oneDataSize <= (int)tempDataList.size()) {
					//�p�C�v�ɑ���f�[�^�̍쐬
					NamedPipe::PipeData pipeData;
					pipeData.byteSize=oneDataSize;
					memcpy(&pipeData.data, &tempDataList[0], oneDataSize);
					pipeDataList->push(pipeData);
					tempDataList.erase(tempDataList.begin(), tempDataList.begin() + pipeData.byteSize);
				}
			}
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
