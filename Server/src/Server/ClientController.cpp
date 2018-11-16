#include"../Include.h"
#include"../Library/ExtensionMutex.h"
#include"../Library/Data.h"
#include"../Library/Cipher/OpenSSLAES.h"
#include"../Library/Cipher/OpenSSLDH.h"
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
	/*�X���b�h�I������*/
	thread->detach();

	/*�f���[�g����*/
	room_numberlist.clear();
	while (1) {
		Client* dele=socketlist[0];
		delete dele;
		MUTEX.Lock();
		socketlist.erase(socketlist.begin());
		MUTEX.Unlock();
		if (socketlist.empty())break;
	}
}

void ClientController::SetSocket(Client* _socket)
{
	socketlist.push_back(_socket);						//�\�P�b�g�̉ϒ��z��ɑ��
}

bool ClientController::SerchNumber(int _number)
{
	for (auto element : socketlist) {
		if (element->GetRoomNumber() == _number) {		
			return true;								//�������[���i���o�[����������
		}
	}
	return false;
}

void ClientController::StartThread(ClientController* _socket_controller)
{
	thread = new std::thread(ControllerThreadLauncher,(void*)_socket_controller);		//�X���b�h�J�n
}


void ClientController::ControllerThread()
{
	while (1) {
		if (socketlist.empty()==false) {
			/*���S�f�[�^�̏���*/
			for (auto client : socketlist) {
				if (client->EmptyCompleteData() == true)break;				//���S�f�[�^���Ȃ���Έȉ������͍s��Ȃ�
				MUTEX.Lock();
				std::vector<char>data = *client->GetCompleteData();			//�e�X���b�h�̎����������f�[�^���擾
				client->DeleteCompleteData();								//���̔z�񂩂�擾���̃f�[�^���폜
				MUTEX.Unlock();
				DataManipulate(client, &data);								//�f�[�^�̉��H���������ꂼ��̏������s��
			}

			/*�ؒf�������s��ꂽ�\�P�b�g���폜*/
			int count = 0;
			for (auto client : socketlist) {
				if (client->GetState() == -1) {
					Client* deletesock;
					deletesock = socketlist[count];
					MUTEX.Lock();
					socketlist.erase(socketlist.begin() + count);
					MUTEX.Unlock();
					delete deletesock;
					break;
				}
				count++;
			}

		}
	}
}


void ClientController::DataManipulate(Client* _socket, std::vector<char>* _data)
{
	char id = *(char*)&_data->at(0);
	switch (id) {

	/*���W�X�V*/
	case 0x15: {
		char encode[BYTESIZE];																		//�Í����f�[�^������
		char senddata[BYTESIZE];																		//���M�f�[�^


		//printf("\n�󂯎�����f�[�^\nid=0x15\n");
		/*�f�[�^�̐��`�����l���Z�b�g*/
		float recvdata = *(float*)&_data->at(sizeof(char));
		_socket->GetData()->SetX(recvdata);
		//printf("x=%f\n", recvdata);
		recvdata = *(float*)&_data->at(sizeof(char) + sizeof(float) * 1);
		_socket->GetData()->SetY(recvdata);
		//printf("y=%f\n", recvdata);
		recvdata = *(float*)&_data->at(sizeof(char) + sizeof(float) * 2);
		_socket->GetData()->SetZ(recvdata);
		//printf("z=%f\n", recvdata);
		recvdata = *(float*)&_data->at(sizeof(char) + sizeof(float) * 3);
		_socket->GetData()->SetAngle(recvdata);
		//printf("angle=%f\n", recvdata);
		int recvdata_int = *(int*)&_data->at(sizeof(char) + sizeof(float) * 4);
		_socket->GetData()->SetAnimation(recvdata_int);
		//printf("animation=%d\n", recvdata_int);

		/*���M�f�[�^�̍쐬*/
		PosData data;
		data.size = sizeof(PosData) - sizeof(int);
		data.id = 0x16;
		data.x = _socket->GetData()->GetX();
		data.y = _socket->GetData()->GetY();
		data.z = _socket->GetData()->GetZ();
		data.angle = _socket->GetData()->GetAngle();
		data.animation = _socket->GetData()->GetAnimation();

		/*�Í�������*/
		char* origin = (char*)&data;
		int encode_size = CIPHER.GetOpenSSLAES()->Encode(encode, origin, sizeof(PosData));		//�Í���
		memcpy(senddata, &encode_size, sizeof(int));
		memcpy(&senddata[sizeof(int)], encode, encode_size);

		/*���M����*/
		send(_socket->GetSocket(), senddata,sizeof(int)+encode_size, 0);						//�쐬�����f�[�^�̍쐬
		//printf("���M�f�[�^\nsize=%d\nid=%04x\nx=%f\ny=%f\nz=%f\nangle=%f\nanimation=%d\n", data.size, data.id, data.x, data.y, data.z, data.angle, data.animation);
		break;
	}

	/*�U������*/
	case 0x17:
		char encode[BYTESIZE];																				//�Í��f�[�^
		char senddata[BYTESIZE];																				//���M�f�[�^

		/*���͎g���ĂȂ���������PC�ƂȂ����ꍇ�������g�p���邱�ƂɂȂ�*/
		int recvdata = *(int*)&_data->at(sizeof(char));													//�N���U�����ꂽ�̂������̒��ɓ����Ă���

		/*���M�f�[�^�̍쐬*/
		BaseData sendbuf;
		sendbuf.size = sizeof(BaseData) - sizeof(int);
		sendbuf.id = 0x18;

		/*�Í�������*/
		int encode_size = CIPHER.GetOpenSSLAES()->Encode(encode, (char*)&sendbuf, sizeof(BaseData));	//�Í�������
		memcpy(senddata, &encode_size, sizeof(int));
		memcpy(&senddata[sizeof(int)], encode, encode_size);
		
		/*���M����*/
		send(_socket->GetSocket(), (char*)&senddata,encode_size+sizeof(int), 0);				//�쐬�����f�[�^�̍쐬
		break;
	}
}


