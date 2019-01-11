#include"../Include.h"
#include"LoadBalancer.h"

void CreateProcessThread(std::string _commando, std::vector<PROCESS_INFORMATION>* _processList);
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);

LoadBalancer::LoadBalancer()
{
}

LoadBalancer::~LoadBalancer()
{
	//serverListのデータ一覧表示
	for (int i = 0; i < serverAmount; i++) {
		std::stringstream query;
		query << PORTNUMBER + i;
		printf("ポート番号:%s,アクセス数=%d\n",query.str().c_str(), serverList[query.str()]);
	}

	//プロセス削除
	for (auto process : processList) {
		EnumWindows(EnumWindowsProc, (LPARAM)&process);

	}
}

void LoadBalancer::Updata()
{
	for (int i = 0; i < 2; i++) {
		//サーバー作成
		std::stringstream query;
		query << "Server.exe " << PORTNUMBER + serverAmount;								//コマンドラインMS作成
		std::thread thread(CreateProcessThread, query.str(), &processList);
		//std::thread thread(CreateProcessThread, "./GameServer/Server.exe", &processList);
		thread.detach();																	//
		
		//queryのバッファーの削除
		query.str("");
		query.clear(std::stringstream::goodbit);

		//サーバー情報の追加
		query << PORTNUMBER + serverAmount;
		serverList.insert(std::make_pair(query.str(), 0));
		serverAmount++;
		Sleep(1000);
	}
}


void LoadBalancer::CreateServer()
{
	//thread[i] = std::make_unique<std::thread>(CreateProcessThread, "notepad.exe");
	//thread[i] = std::make_unique<std::thread>(CreateProcessThread, "./GameServer/Server.exe");

	//プロセス作成
	for (int i = 0; i < 5; i++) {
		std::thread thread(CreateProcessThread, "notepad.exe", &processList);
		thread.detach();
		Sleep(1000);
	}

}


//プロセス作成用のスレッド
void CreateProcessThread(std::string _commando, std::vector<PROCESS_INFORMATION>* _processList)
{
	PROCESS_INFORMATION process;
	STARTUPINFO startupInfo;
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);

	//メモ帳を実行
	if (!CreateProcess(
		NULL,				// 実行可能モジュールの名
		//(LPSTR)"./GameServer/Server.exe", // コマンドラインの文字列
		(LPSTR)_commando.c_str(),
		NULL, // セキュリティ記述子
		NULL,// セキュリティ記述子
		FALSE, // ハンドルの継承オプション
		NULL, // 作成のフラグ
		NULL,// 新しい環境ブロック
		NULL, // カレントディレクトリの名前
		&startupInfo, // スタートアップ情報
		&process // プロセス情報
	)) {
		printf("失敗");
		return;
	}
	_processList->push_back(process);
	WaitForSingleObject(process.hProcess, INFINITE);
	return;
}

//EnumWindowのコールバック関数
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	// CreateProcess()で取得したPROCESS_INFORMATION構造体のポインタを取得
	PROCESS_INFORMATION* pi = (PROCESS_INFORMATION*)lParam;

	// ウインドウを作成したプロセスIDを取得。
	DWORD lpdwProcessId = 0;
	::GetWindowThreadProcessId(hWnd, &lpdwProcessId);

	// CreateProcessで起動したアプリのプロセスIDとメインウィンドウを
	// 作成したプロセスIDが同じ場合、起動したアプリを終了させる。
	if (pi->dwProcessId == lpdwProcessId)
	{
		::PostMessage(hWnd, WM_CLOSE, 0, 0);
		return FALSE;
	}
	return TRUE;
}
