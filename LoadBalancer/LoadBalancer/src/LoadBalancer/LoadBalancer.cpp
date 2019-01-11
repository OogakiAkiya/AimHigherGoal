#include"../Include.h"
#include"LoadBalancer.h"

void CreateProcessThread(std::string _commando, std::vector<PROCESS_INFORMATION>* _processList);
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);

LoadBalancer::LoadBalancer()
{
}

LoadBalancer::~LoadBalancer()
{
	//serverList�̃f�[�^�ꗗ�\��
	for (int i = 0; i < serverAmount; i++) {
		std::stringstream query;
		query << PORTNUMBER + i;
		printf("�|�[�g�ԍ�:%s,�A�N�Z�X��=%d\n",query.str().c_str(), serverList[query.str()]);
	}

	//�v���Z�X�폜
	for (auto process : processList) {
		EnumWindows(EnumWindowsProc, (LPARAM)&process);

	}
}

void LoadBalancer::Updata()
{
	for (int i = 0; i < 2; i++) {
		//�T�[�o�[�쐬
		std::stringstream query;
		query << "Server.exe " << PORTNUMBER + serverAmount;								//�R�}���h���C��MS�쐬
		std::thread thread(CreateProcessThread, query.str(), &processList);
		//std::thread thread(CreateProcessThread, "./GameServer/Server.exe", &processList);
		thread.detach();																	//
		
		//query�̃o�b�t�@�[�̍폜
		query.str("");
		query.clear(std::stringstream::goodbit);

		//�T�[�o�[���̒ǉ�
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

	//�v���Z�X�쐬
	for (int i = 0; i < 5; i++) {
		std::thread thread(CreateProcessThread, "notepad.exe", &processList);
		thread.detach();
		Sleep(1000);
	}

}


//�v���Z�X�쐬�p�̃X���b�h
void CreateProcessThread(std::string _commando, std::vector<PROCESS_INFORMATION>* _processList)
{
	PROCESS_INFORMATION process;
	STARTUPINFO startupInfo;
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);

	//�����������s
	if (!CreateProcess(
		NULL,				// ���s�\���W���[���̖�
		//(LPSTR)"./GameServer/Server.exe", // �R�}���h���C���̕�����
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
