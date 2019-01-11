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
	//�v���Z�X�폜
	for (auto process : processList) {
		EnumWindows(EnumWindowsProc, (LPARAM)&process);
	}

}

void Process::CreateProcessThread(std::string _commando)
{
	//thread[i] = std::make_unique<std::thread>(CreateProcessThread, "notepad.exe");
	//thread[i] = std::make_unique<std::thread>(CreateProcessThread, "./GameServer/Server.exe");

	//�v���Z�X�쐬
	std::thread thread(ProcessThread, _commando, &processList);
	thread.detach();
	processAmount++;

}

int Process::GetProcessAmount()
{
	return processAmount;
}


//�v���Z�X�쐬�p�̃X���b�h
void ProcessThread(std::string _commando, std::vector<PROCESS_INFORMATION>* _processList)
{
	PROCESS_INFORMATION process;
	STARTUPINFO startupInfo;
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);

	if (!CreateProcess(
		NULL,				// ���s�\���W���[���̖�
		(LPSTR)_commando.c_str(),
		NULL, // �Z�L�����e�B�L�q�q
		NULL,// �Z�L�����e�B�L�q�q
		FALSE, // �n���h���̌p���I�v�V����
		NULL, // �쐬�̃t���O
		NULL,// �V�������u���b�N
		NULL, // �J�����g�f�B���N�g���̖��O
		&startupInfo, // �X�^�[�g�A�b�v���
		&process // �v���Z�X���
	)) {
		printf("���s");
		return;
	}
	_processList->push_back(process);
	WaitForSingleObject(process.hProcess, INFINITE);
	return;
}

//EnumWindow�̃R�[���o�b�N�֐�
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	// CreateProcess()�Ŏ擾����PROCESS_INFORMATION�\���̂̃|�C���^���擾
	PROCESS_INFORMATION* pi = (PROCESS_INFORMATION*)lParam;

	// �E�C���h�E���쐬�����v���Z�XID���擾�B
	DWORD lpdwProcessId = 0;
	::GetWindowThreadProcessId(hWnd, &lpdwProcessId);

	// CreateProcess�ŋN�������A�v���̃v���Z�XID�ƃ��C���E�B���h�E��
	// �쐬�����v���Z�XID�������ꍇ�A�N�������A�v�����I��������B
	if (pi->dwProcessId == lpdwProcessId)
	{
		::PostMessage(hWnd, WM_CLOSE, 0, 0);
		return FALSE;
	}
	return TRUE;
}
