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
	while (1) {
		if (tempDataQueue->empty())break;
		//�f�[�^����
		Header header = *(Header*)&tempDataQueue->front().data[0];
		DataProcess(&header);
		//�ꎞ�f�[�^�̍폜
		tempDataQueue->pop();

	}
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

void Client::DataProcess(Header* _header)
{
	switch (_header->id) {
	//�f�[�^�ׁ[�X����f�[�^�̎擾
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
		char oriData[BYTESIZE];
		char decodeData[BYTESIZE];
		int  decodeSize = _header->size - sizeof(Header);

		//��������
		memcpy(&oriData, &tempDataQueue->front().data[sizeof(Header)], decodeSize);
		int byteSize = aes->Decode(decodeData, oriData, decodeSize);

		//�f�[�^�̍X�V����
		data->SetX(*(float*)&decodeData[0]);
		data->SetY(*(float*)&decodeData[sizeof(float) * 1]);
		data->SetZ(*(float*)&decodeData[sizeof(float) * 2]);
		data->SetAngle(*(float*)&decodeData[sizeof(float) * 3]);
		data->SetAnimation(*(int*)&decodeData[sizeof(float) * 4]);

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
	//������
	case 0xFF: {
		char decodeData[BYTESIZE];
		int decodeSize = tempDataQueue->front().byteSize - sizeof(Header);
		int outLen = CIPHER.GetOpenSSLRSA()->Decode(decodeData, &tempDataQueue->front().data[sizeof(Header)], decodeSize);						//���J���Í��̕���
		aes->SetKey((unsigned char*)decodeData, outLen);												//���ʌ���ݒ�
		break;
	}
	//�z��O�̃f�[�^��������
	default:
		break;
	}

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
	Header headerData;
	memcpy(oridata, _originalData, _dataLen);
	//�Í���
	if (_dataLen > 0) {
		encodeSize = aes->Encode(encode, &oridata[0], _dataLen);				//�Í���
	}

	//�w�b�_�[���쐬
	headerData.size = sizeof(Header)+encodeSize;
	headerData.playerIdSize = data->GetId()->length();
	memcpy(headerData.playerId, data->GetId()->c_str(), headerData.playerIdSize);
	headerData.id = _id;


	NamedPipe::PipeData sendData;
	sendData.byteSize = headerData.size;
	memcpy(sendData.data, &headerData, sizeof(Header));
	memcpy(&sendData.data[sizeof(Header)], encode, encodeSize);
	completeDataQueue->push(sendData);
}
