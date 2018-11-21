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
Client* Client::s_Instance = nullptr;

Client::Client()
{
	/*�C���X�^���X�̐���*/
	cipher = new Cipher();										//�Í������p�N���X
	mutex = new ExtensionMutex();								//�r������p�N���X
	socket = Socket::Instantiate()->
		SetProtocolVersion_Dual().
		SetProtocol_TCP().
		SetIpAddress("localhost").
		SetPortNumber("49155").
		ClientCreate();
	
	char keyBuf[EVP_MAX_KEY_LENGTH];
	char endata[BYTESIZE];
	char sendbuf[BYTESIZE];
	cipher->GetOpenSSLAES()->GetKey(keyBuf);
	int outlen=cipher->GetOpenSSLRSA()->Encode(endata, keyBuf, EVP_MAX_KEY_LENGTH);
	memcpy(sendbuf, &outlen, sizeof(int));
	memcpy(&sendbuf[sizeof(int)], &endata, outlen);
	send(socket->GetSocket(), sendbuf,sizeof(int)+outlen,0);
}

Client::~Client()
{
	/*�������*/
	socket->Close();
	thread->detach();
	delete mutex;
	delete cipher;
	delete socket;
}


void Client::StartThread()
{
	thread = new std::thread(ClientThreadLauncher, (void*)s_Instance);
}

void Client::Recv()
{
	/*�ϐ��쐬*/
	int iResult;																							//recv���ʂ�����(��M�����o�C�g��������)
	char rec[BYTESIZE];																							//�󂯎�����f�[�^���i�[
	
	/*��M*/
	while (1) {
		iResult = socket->Recv(rec,BYTESIZE);
		if (iResult > 0) {

			/*��M�f�[�^���ꎞ�f�[�^�z��ɒǉ�*/
			int now_size = temp_datalist.size();															//�ꎞ�f�[�^�z��ɉ�byte�f�[�^�������Ă��邩������
			temp_datalist.resize(now_size + iResult);														//�����Ă����f�[�^���i�[�ł���悤�Ɉꎞ�f�[�^�z��̃T�C�Y�ύX
			memcpy((char*)&temp_datalist[now_size], rec, iResult);											//�Ō���ɑ����Ă����f�[�^�̒ǉ�

			/*�ꎞ�f�[�^���犮�S�f�[�^�̍쐬*/
			while (temp_datalist.size() >= 4) {																//��byte�̃f�[�^�������Ă��Ă����邩����ǂݍ��߂Ȃ���Δ�����
				
				/*��������*/
				int decodesize = *(int*)&temp_datalist[0];
				if (decodesize <= (int)temp_datalist.size() - 4) {
					char data[BYTESIZE];																	//�����O�f�[�^
					char decode_data[BYTESIZE];																//�����f�[�^
					memcpy(data, &temp_datalist[sizeof(int)], decodesize);
					cipher->GetOpenSSLAES()->Decode(decode_data, data, decodesize);
					int byteSize = *(int*)decode_data;														//4byte�������擾��int�̒l�ɃL���X�g
					std::vector<char> compData(byteSize);													//���S�f�[�^
					memcpy(&compData[0], &decode_data[sizeof(int)], byteSize);								//�T�C�Y�ȊO�̃f�[�^���g�p�����S�f�[�^���쐬
					temp_datalist.erase(temp_datalist.begin(), temp_datalist.begin() + (decodesize + 4));	//���S�f�[�^�쐬�Ɏg�p���������폜

					/*���S�f�[�^�̏���*/
					DataManipulate(&compData);
				}
				else {
					/*���S�f�[�^�쐬�s�\�ɂȂ����ꍇ*/
					break;
				}
			}

		}
		else if (iResult == 0) {
			/*�f�[�^�������Ă��Ȃ������ꍇ�ؒf����*/
			break;
		}
		else {
			/*�G���[����*/
			int error = WSAGetLastError();
			if (error != 10035) {									//�񓯊��̏ꍇ���̃G���[���Ăяo����邱�Ƃ��悭���邪�v���I�łȂ����߃X���[�����ėǂ�
				break;
			}
		}
	}
}

void Client::SendPos(Data* _data)
{
	/*���M�f�[�^�̐���*/
	PosData data;
	data.size = sizeof(PosData) - sizeof(int);
	data.id = 0x15;
	data.x = _data->GetX();
	data.y = _data->GetY();
	data.z = _data->GetZ();
	data.angle = _data->GetAngle();
	data.animation = _data->GetAnimation();

	/*�Í�����*/
	char* origin = (char*)&data;
	char encode[BYTESIZE];										//�Í����f�[�^������
	char senddata[BYTESIZE];									//���M�f�[�^
	int encode_size = cipher->GetOpenSSLAES()->Encode(encode, origin, sizeof(PosData));
	memcpy(senddata, &encode_size, sizeof(int));
	memcpy(&senddata[sizeof(int)], encode, encode_size);

	/*�f�[�^���M*/
	send(CLIENT.GetSocket(), senddata, sizeof(int) + encode_size, 0);
}

void Client::SendAttack(Data* _data)
{
	/*�����������ǂ����𔻒�*/
	float Lenght;
	D3DXVECTOR3 VLenght;
	D3DXVECTOR3 player_vector(_data->GetX(), _data->GetY(), _data->GetZ());													//�v���C���[�̃x�N�g��
	for (int element = 0; element < 3; element++) {																			//id�͓G�̋��
		D3DXVECTOR3 enemy_vector(enemydata[element].GetX(), enemydata[element].GetY(), enemydata[element].GetZ());			//�G�̃x�N�g��
		VLenght = enemy_vector - player_vector;																				//��̃x�N�g���̍�
		Lenght = D3DXVec3Length(&VLenght);

		/*�f�[�^���M(���������ꍇ)*/
		if (Lenght <= 2.0f) {
			char encode[BYTESIZE];									//�Í��f�[�^
			char senddata[BYTESIZE];								//���M�f�[�^

			/*�f�[�^�̐���*/
			AttckData data;
			data.base.size = sizeof(AttckData) - sizeof(int);
			data.base.id = 0x17;
			data.socket = element;
			
			/*�Í�����*/
			int encode_size = cipher->GetOpenSSLAES()->Encode(encode, (char*)&data, sizeof(AttckData));
			memcpy(senddata, &encode_size, sizeof(int));
			memcpy(&senddata[sizeof(int)], encode, encode_size);
			/*���M����*/
			send(CLIENT.GetSocket(), (char*)&senddata, sizeof(int) + encode_size, 0);
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
	return data_qlist.front();
}

void Client::DeleteData()
{
	data_qlist.pop();
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
	if (data_qlist.empty() == true)return true;
	return false;
}

void Client::SetEnemyData(int _enemyid, Data * data)
{
	enemydata[_enemyid] = *data;
}

void Client::Lock()
{
	mutex->Lock();
}

void Client::Unlock()
{
	mutex->Unlock();
}

Client & Client::GetInstance()
{
	if (s_Instance == nullptr) { s_Instance = new Client(); }
	return *s_Instance;
}

void Client::DeleteInstance()
{
	if (s_Instance) {
		delete s_Instance;
		s_Instance = nullptr;
	}
}


void Client::DataManipulate(const std::vector<char>* _data)
{
	Data data;
	char id = *(char*)&_data->at(0);					//�Ȃ�̃f�[�^��id�Ŕ��f����

	switch (id) {
	/*���W�X�V����*/
	case 0x16: {
		float recvdata = *(float*)&_data->at(sizeof(char) + sizeof(float) * 0);
		data.SetX(recvdata);
		recvdata = *(float*)&_data->at(sizeof(char) + sizeof(float) * 1);
		data.SetY(recvdata);
		recvdata = *(float*)&_data->at(sizeof(char) + sizeof(float) * 2);
		data.SetZ(recvdata);
		recvdata = *(float*)&_data->at(sizeof(char) + sizeof(float) * 3);
		data.SetAngle(recvdata);
		recvdata = *(int*)&_data->at(sizeof(char) + sizeof(float) * 3 + sizeof(int));
		data.SetAnimation(recvdata);
		Lock();
		data_qlist.push(data);
		Unlock();
		break;
	}

	/*�U������*/
	case 0x18:
		data.SetX(0.0f);					//�G�̌��݂̍��W
		data.SetY(0.0f);
		data.SetZ(0.0f);
		data.SetAngle(0);
		data.SetAnimation(DAMAGE);
		Lock();
		data_qlist.push(data);
		Unlock();
		break;
	}

}

