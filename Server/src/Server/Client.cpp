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
	/*
	//鍵交換
	if (!keyChangeFlg) {
		RecvLoop(EXCHANGEKEY);
	}
	//完全データ作成処理
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
		//復号に十分なデータがあるかチェック
		int decodeSize = *(int*)&tempDataList[0];
		if (decodeSize <= (int)tempDataList.size() - sizeof(int)) {

			//変数宣言
			char data[BYTESIZE];																			//復号前データ
			char decodeData[BYTESIZE];

			//復号処理
			memcpy(data, &tempDataList[sizeof(int)], decodeSize);											//復号するデータのコピー
			int outLen = CIPHER.GetOpenSSLRSA()->Decode(decodeData, data, decodeSize);						//公開鍵暗号の復号
			aes->SetKey((unsigned char*)decodeData, outLen);												//共通鍵を設定
			tempDataList.erase(tempDataList.begin(), tempDataList.begin() + (decodeSize + sizeof(int)));	//完全データ作成に使用した分を削除
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

		//鍵交換
		if (!keyChangeFlg) {
			BaseData originData = *(BaseData*)&tempDataQueue->front().data[0];

			int decodeSize = tempDataQueue->front().byteSize - sizeof(BaseData);
			int outLen = CIPHER.GetOpenSSLRSA()->Decode(decodeData, &tempDataQueue->front().data[sizeof(BaseData)], decodeSize);						//公開鍵暗号の復号
			aes->SetKey((unsigned char*)decodeData, outLen);												//共通鍵を設定
			tempDataQueue->pop();
			keyChangeFlg = true;
			return;
		}

		//復号処理
		BaseData originData = *(BaseData*)&tempDataQueue->front().data[0];
		char oriData[BYTESIZE];
		int  decodeSize=originData.size - sizeof(BaseData);
		memcpy(&oriData, &tempDataQueue->front().data[sizeof(BaseData)], decodeSize);
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

		char recvData[BYTESIZE];																	//データベースから取得した値が入る

		//データの更新処理
		data->SetX(*(float*)&_data[sizeof(char)]);
		data->SetY(*(float*)&_data[sizeof(char) + sizeof(float) * 1]);
		data->SetZ(*(float*)&_data[sizeof(char) + sizeof(float) * 2]);
		data->SetAngle(*(float*)&_data[sizeof(char) + sizeof(float) * 3]);
		data->SetAnimation(*(int*)&_data[sizeof(char) + sizeof(float) * 4]);

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
	}

}

//クライアント全ての座標更新
void Client::DBCreateData()
{
	/*
	std::stringstream query;						//webサーバーに送るデータ
	std::string output;								//queryのままだとエラーが起こりstring型に入れるとなくなる
	std::vector<std::shared_ptr<Data>> datas;		//送信するデータ一覧

	//送るデータの選別
	for (auto socket : clientList) {
		if (socket->GetPosGetFlg()) {
			datas.push_back(socket->GetData());
		}
	}
	if (datas.size() <= 0)return;

	//データの作成
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
	BaseData headerData;
	memcpy(oridata, _originalData, _dataLen);
	//暗号化
	if (_dataLen > 0) {
		encodeSize = aes->Encode(encode, &oridata[0], _dataLen);				//暗号化
	}

	//ヘッダー部作成
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
