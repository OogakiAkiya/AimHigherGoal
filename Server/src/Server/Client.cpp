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
	/*�������*/
	thread->join();										//�X���b�h�I��
	delete data;
	delete curl;
	delete thread;
	delete aes;
}



void Client::Recv()
{
	//������
	while (1) {
		int iResult;										//�����Ă����f�[�^�ʂ��i�[�����
		char rec[BYTESIZE];									//��M�f�[�^
		bool exitflg = false;

		/*��M*/
		iResult = recv(data->GetSocket(), rec/*�f�[�^*/, sizeof(rec), 0);
		if (iResult > 0) {

			/*��M�f�[�^���ꎞ�f�[�^�z��ɒǉ�*/
			int now_size = temp_datalist.size();															//�ꎞ�f�[�^�z��ɉ�byte�f�[�^�������Ă��邩������
			temp_datalist.resize(now_size + iResult);														//�����Ă����f�[�^���i�[�ł���悤�Ɉꎞ�f�[�^�z��̃T�C�Y�ύX
			memcpy((char*)&temp_datalist[now_size], rec, iResult);											//�Ō���ɑ����Ă����f�[�^�̒ǉ�

			/*�ꎞ�f�[�^���犮�S�f�[�^�̍쐬*/
			while (temp_datalist.size() >= sizeof(int)) {																//��byte�̃f�[�^�������Ă��Ă����邩����ǂݍ��߂Ȃ���Δ�����

				
				int decodesize = *(int*)&temp_datalist[0];
				if (decodesize <= (int)temp_datalist.size() - sizeof(int)) {
					char data[BYTESIZE];							//�����O�f�[�^
					char decode_data[BYTESIZE];
					memcpy(data, &temp_datalist[sizeof(int)], decodesize);
					int outlen=CIPHER.GetOpenSSLRSA()->Decode(decode_data, data, decodesize);
					aes->SetKey((unsigned char*)decode_data, outlen);
					temp_datalist.erase(temp_datalist.begin(), temp_datalist.begin() + (decodesize + sizeof(int)));	//���S�f�[�^�쐬�Ɏg�p���������폜
					exitflg = true;
					break;
				}
				else {
					break;
				}
			}
			if (exitflg)break;
		}
		else if (iResult == 0) {
			/*�ڑ����I������Ƃ�*/
			printf("�ؒf����܂���\n");
			state = -1;
			return;

		}
		else {
			/*�ڑ��G���[���N��������*/
			printf("recv failed:%d\n%d", WSAGetLastError(), iResult);
			state = -1;
			return;

		}

	}

	//�Q�[������
	while (1) {
		int iResult;										//�����Ă����f�[�^�ʂ��i�[�����
		char rec[BYTESIZE];									//��M�f�[�^

		/*��M*/
		iResult = recv(data->GetSocket()/*���M���̃\�P�b�g*/, rec/*�f�[�^*/, sizeof(rec), 0);
		if (iResult > 0) {

			/*��M�f�[�^���ꎞ�f�[�^�z��ɒǉ�*/
			int now_size = temp_datalist.size();															//�ꎞ�f�[�^�z��ɉ�byte�f�[�^�������Ă��邩������
			temp_datalist.resize(now_size + iResult);														//�����Ă����f�[�^���i�[�ł���悤�Ɉꎞ�f�[�^�z��̃T�C�Y�ύX
			memcpy((char*)&temp_datalist[now_size], rec, iResult);											//�Ō���ɑ����Ă����f�[�^�̒ǉ�

			/*�ꎞ�f�[�^���犮�S�f�[�^�̍쐬*/
			while (temp_datalist.size() >= 4) {																//��byte�̃f�[�^�������Ă��Ă����邩����ǂݍ��߂Ȃ���Δ�����
				
				/*�Í������ꂽ�f�[�^�̕�������*/
				int decodesize = *(int*)&temp_datalist[0];
				if (decodesize <= (int)temp_datalist.size() - 4) {
					char data[BYTESIZE];							//�����O�f�[�^
					char decode_data[BYTESIZE];
					memcpy(data, &temp_datalist[sizeof(int)], decodesize);
					aes->Decode(decode_data,data,decodesize);

					/*���S�f�[�^�̐���*/
					int byteSize = *(int*)decode_data;														//4byte�������擾��int�̒l�ɃL���X�g
					std::vector<char> compData(byteSize);													//���S�f�[�^
					memcpy(&compData[0], &decode_data[sizeof(int)], byteSize);								//�T�C�Y�ȊO�̃f�[�^���g�p�����S�f�[�^���쐬
					MUTEX.Lock();
					completedata_qlist.push(compData);														//���S�f�[�^�z��Ɋi�[
					MUTEX.Unlock();
					temp_datalist.erase(temp_datalist.begin(), temp_datalist.begin() + (decodesize + sizeof(int)));	//���S�f�[�^�쐬�Ɏg�p���������폜

				}else {
					break;
				}
			}

		}else if (iResult == 0) {
			/*�ڑ����I������Ƃ�*/
			printf("�ؒf����܂���\n");
			state = -1;
			break;

		}else {
			/*�ڑ��G���[���N��������*/
			printf("recv failed:%d\n%d", WSAGetLastError(), iResult);
			state = -1;
			break;

		}
	}
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
	return room_number;
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
	room_number = _number;
}


std::vector<char>* Client::GetCompleteData()
{
	return &completedata_qlist.front();
}

void Client::DeleteCompleteData()
{
	completedata_qlist.pop();
}

bool Client::EmptyCompleteData()
{
	if (completedata_qlist.empty() == true) {
		return true;									//��
	}
	return false;										//�l�������Ă���
}

