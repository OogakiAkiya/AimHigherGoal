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


std::queue<NamedPipe::PipeData>* Client::GetSendData()
{
	return completeDataQueue.get();
}

void Client::AddData(NamedPipe::PipeData* _data)
{
	tempDataQueue->push(*_data);
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

void Client::DecryptionManipulate()
{
	while (1) {
		if (tempDataQueue->empty())break;
		NamedPipe::PipeData sendData;
		char decodeData[BYTESIZE];

		//������
		if (!keyChangeFlg) {
			BaseData originData = *(BaseData*)&tempDataQueue->front().data[0];

			int decodeSize = tempDataQueue->front().byteSize - sizeof(BaseData);
			int outLen = CIPHER.GetOpenSSLRSA()->Decode(decodeData, &tempDataQueue->front().data[sizeof(BaseData)], decodeSize);						//���J���Í��̕���
			aes->SetKey((unsigned char*)decodeData, outLen);												//���ʌ���ݒ�
			tempDataQueue->pop();
			keyChangeFlg = true;
			return;
		}

		//��������
		BaseData originData = *(BaseData*)&tempDataQueue->front().data[0];
		char oriData[BYTESIZE];
		int  decodeSize=originData.size - sizeof(BaseData);
		memcpy(&oriData, &tempDataQueue->front().data[sizeof(BaseData)], decodeSize);
		int byteSize=aes->Decode(decodeData, oriData, decodeSize);
		tempDataQueue->pop();
		//�f�[�^����
		DataManipulater(originData.id, decodeData);
	}
}

void Client::DataManipulater(char _id,char* _data)
{
	switch (_id) {
	case 0x01: {
		char recvData[BYTESIZE];																	//�f�[�^�x�[�X����擾�����l������

		//�v���C���[�̍��W�擾
		DBGetPos(recvData, data->GetId());

		//�擾�����f�[�^���Z�b�g
		data->SetX(*(float*)recvData);
		data->SetY(*(float*)&recvData[sizeof(float)]);
		data->SetZ(*(float*)&recvData[sizeof(float) * 2]);

		//���M�f�[�^�쐬
		Pos pos = { data->GetX(), data->GetY(), data->GetZ() };
		CreateSendData((char*)&pos, sizeof(Pos), 0x02);
		SetPosGetFlg();
		break;
	}
			   //���W�X�V
	case 0x15: {

		char recvData[BYTESIZE];																	//�f�[�^�x�[�X����擾�����l������

		//�f�[�^�̍X�V����
		data->SetX(*(float*)&_data[sizeof(char)]);
		data->SetY(*(float*)&_data[sizeof(char) + sizeof(float) * 1]);
		data->SetZ(*(float*)&_data[sizeof(char) + sizeof(float) * 2]);
		data->SetAngle(*(float*)&_data[sizeof(char) + sizeof(float) * 3]);
		data->SetAnimation(*(int*)&_data[sizeof(char) + sizeof(float) * 4]);

		//���M�f�[�^�쐬
		UserData userData = { data->GetX(), data->GetY(), data->GetZ(),data->GetAngle(),data->GetAnimation() };
		CreateSendData((char*)&userData, sizeof(UserData), 0x16);

		break;
	}
	//�U������
	case 0x17:
		/*
		char sendData[BYTESIZE];																				//���M�f�[�^

		//���͎g���ĂȂ���������PC�ƂȂ����ꍇ�������g�p���邱�ƂɂȂ�
		int recvData = *(int*)&_data->at(sizeof(char));													//�N���U�����ꂽ�̂������̒��ɓ����Ă���

		//���M�f�[�^�̍쐬
		tempData sendBuf;
		sendBuf.size = sizeof(tempData) - sizeof(int);
		sendBuf.id = 0x18;

		//���M�p�f�[�^�̍쐬
		int amountSize = CreateSendData(sendData, _socket, (char*)&sendBuf, sizeof(tempData));

		//���M����
		send(_socket->GetSocket(), (char*)&sendData, amountSize, 0);
		*/
		break;
	}

}

//�N���C�A���g�S�Ă̍��W�X�V
void Client::DBCreateData()
{
	/*
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
	*/

}

void Client::DBGetPos(char * _data, std::shared_ptr<std::string> _userId)
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
	curl->HTTPConnect(&buf, "http://lifestyle-qa.com/get_pos.php", output.c_str(), *_userId.get());
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

void Client::CreateSendData(char * _originalData, int _dataLen,char _id)
{
	int encodeSize=0;
	char encode[BYTESIZE];																		//�Í����f�[�^
	char oridata[BYTESIZE];
	BaseData headerData;
	memcpy(oridata, _originalData, _dataLen);
	//�Í���
	if (_dataLen > 0) {
		encodeSize = aes->Encode(encode, &oridata[0], _dataLen);				//�Í���
	}

	//�w�b�_�[���쐬
	headerData.size = sizeof(BaseData)+encodeSize;
	headerData.playerIdSize = data->GetId()->length();
	memcpy(headerData.playerId, data->GetId()->c_str(), headerData.playerIdSize);
	headerData.id = _id;


	NamedPipe::PipeData sendData;
	sendData.byteSize = headerData.size;
	memcpy(sendData.data, &headerData, sizeof(BaseData));
	memcpy(&sendData.data[sizeof(BaseData)], encode, encodeSize);
	completeDataQueue->push(sendData);
}
