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
	tempDataQueue= std::make_unique<std::queue<NamedPipe::PipeData>>();			//デコード前のデータ
	completeDataQueue=std::make_unique<std::queue<NamedPipe::PipeData>>();		//送信データ

	data = std::make_shared<Data>();
	aes = std::make_unique<OpenSSLAES>();
}

Client::~Client()
{
	while (1) {
		if (tempDataQueue->empty())break;
		tempDataQueue->pop();
	}
	tempDataQueue = nullptr;			//デコード前のデータ
	while (1) {
		if (completeDataQueue->empty())break;
		completeDataQueue->pop();
	}
	completeDataQueue = nullptr;		//送信データ

	//解放処理
	aes=nullptr;
	data = nullptr;

}

void Client::Update()
{
	DecryptionManipulate();
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


bool Client::ExchangeKey(char* _decodeData)
{
	int decodeSize = tempDataQueue->front().byteSize - sizeof(Header);
	int outLen = CIPHER.GetOpenSSLRSA()->Decode(_decodeData, &tempDataQueue->front().data[sizeof(Header)], decodeSize);						//公開鍵暗号の復号
	aes->SetKey((unsigned char*)_decodeData, outLen);												//共通鍵を設定
	tempDataQueue->pop();
	keyChangeFlg = true;
	return true;
}

void Client::DecryptionManipulate()
{
	while (1) {
		if (tempDataQueue->empty())break;
		NamedPipe::PipeData sendData;
		char decodeData[BYTESIZE];

		Header originData = *(Header*)&tempDataQueue->front().data[0];
		
		//鍵交換
		if (originData.id == 0x20) {
			ExchangeKey(decodeData);
			return;
		}

		char oriData[BYTESIZE];
		int  decodeSize=originData.size - sizeof(Header);
		//復号処理
		memcpy(&oriData, &tempDataQueue->front().data[sizeof(Header)], decodeSize);
		int byteSize=aes->Decode(decodeData, oriData, decodeSize);
		tempDataQueue->pop();
		//データ処理
		DataManipulater(originData.id, decodeData);
	}
}

void Client::DataManipulater(char _id,char* _data)
{
	switch (_id) {
	case 0x01: {
		char recvData[BYTESIZE];																	//データベースから取得した値が入る

		//プレイヤーの座標取得
		DBGetPos(recvData, data->GetId());

		//取得したデータをセット
		data->SetX(*(float*)recvData);
		data->SetY(*(float*)&recvData[sizeof(float)]);
		data->SetZ(*(float*)&recvData[sizeof(float) * 2]);

		//送信データ作成
		Pos pos = { data->GetX(), data->GetY(), data->GetZ() };
		CreateSendData((char*)&pos, sizeof(Pos), 0x02);
		SetPosGetFlg();
		break;
	}
			   //座標更新
	case 0x15: {

		//データの更新処理
		data->SetX(*(float*)&_data[0]);
		data->SetY(*(float*)&_data[sizeof(float) * 1]);
		data->SetZ(*(float*)&_data[sizeof(float) * 2]);
		data->SetAngle(*(float*)&_data[sizeof(float) * 3]);
		data->SetAnimation(*(int*)&_data[sizeof(float) * 4]);

		//送信データ作成
		UserData userData = { data->GetX(), data->GetY(), data->GetZ(),data->GetAngle(),data->GetAnimation() };
		CreateSendData((char*)&userData, sizeof(UserData), 0x16);

		break;
	}
	//攻撃処理
	case 0x17:
		/*
		char sendData[BYTESIZE];																				//送信データ

		//今は使ってないが複数のPCとつなげた場合ここを使用することになる
		int recvData = *(int*)&_data->at(sizeof(char));													//誰が攻撃されたのかがこの中に入っている

		//送信データの作成
		tempData sendBuf;
		sendBuf.size = sizeof(tempData) - sizeof(int);
		sendBuf.id = 0x18;

		//送信用データの作成
		int amountSize = CreateSendData(sendData, _socket, (char*)&sendBuf, sizeof(tempData));

		//送信処理
		send(_socket->GetSocket(), (char*)&sendData, amountSize, 0);
		*/
		break;
	default:

		break;
	}

}

void Client::DBGetPos(char * _data, std::shared_ptr<std::string> _userId)
{
	//ユーザー追加処理
	std::stringstream query;
	std::string output = "";																//送信用データ
	std::string buf;																		//受け取ったデータを格納する
	std::string error;
	char recvdata[256];

	//送信データの生成
	query << "player=" << _userId.get()->c_str();
	query >> output;

	std::unique_ptr<CurlWrapper> curl = std::make_unique<CurlWrapper>();
	curl->HTTPConnect(&buf, "http://lifestyle-qa.com/get_pos.php", output.c_str(), *_userId.get());
	curl = nullptr;

	//jsonから値の取得
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	if (buf.length() != 0) {
		auto json = json11::Json::parse(buf, error);

		x = std::stof(json["x"].string_value());
		y = std::stof(json["y"].string_value());
		z = std::stof(json["z"].string_value());
	}

	//データ代入
	memcpy(recvdata, &x, sizeof(float));
	memcpy(&recvdata[sizeof(float)], &y, sizeof(float));
	memcpy(&recvdata[sizeof(float) * 2], &z, sizeof(float));
	memcpy(_data, recvdata, sizeof(float) * 3);

}

void Client::CreateSendData(char * _originalData, int _dataLen,char _id)
{
	int encodeSize=0;
	char encode[BYTESIZE];																		//暗号化データ
	char oridata[BYTESIZE];
	Header headerData;
	memcpy(oridata, _originalData, _dataLen);
	//暗号化
	if (_dataLen > 0) {
		encodeSize = aes->Encode(encode, &oridata[0], _dataLen);				//暗号化
	}

	//ヘッダー部作成
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
