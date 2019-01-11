#include <windows.h>
#include <stdio.h>
#include<thread>
#include<vector>
#include<sstream>
#include<map>
#include"Process.h"

void ProcessThread(std::string _commando, std::vector<PROCESS_INFORMATION>* _processList);
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);

Process::Process()
{
}

Process::~Process()
{
	//プロセス削除
	for (auto process : processList) {
		EnumWindows(EnumWindowsProc, (LPARAM)&process);
	}

}

void Process::CreateProcessThread(std::string _commando)
{
	//thread[i] = std::make_unique<std::thread>(CreateProcessThread, "notepad.exe");
	//thread[i] = std::make_unique<std::thread>(CreateProcessThread, "./GameServer/Server.exe");

	//プロセス作成
	std::thread thread(ProcessThread, _commando, &processList);
	thread.detach();
	processAmount++;

}

int Process::GetProcessAmount()
{
	return processAmount;
}


//プロセス作成用のスレッド
void ProcessThread(std::string _commando, std::vector<PROCESS_INFORMATION>* _processList)
{
	PROCESS_INFORMATION process;
	STARTUPINFO startupInfo;
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);

	if (!CreateProcess(
		NULL,				// 実行可能モジュールの名
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
