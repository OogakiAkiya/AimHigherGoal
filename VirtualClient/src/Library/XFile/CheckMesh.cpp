#include"../StandardLibraryInclude.h"
#include"../DirectX/D3D.h"
#include"XFILE.h"
#include"CheckMesh.h"

CheckMesh* CheckMesh::s_Instance = nullptr;

CheckMesh::CheckMesh() {

}

CheckMesh::~CheckMesh(){
}

XFILE* CheckMesh::Get(std::string _pass) {

	//���X�g���ɂ��邩�T��
	if (meshList.find(_pass) != meshList.end()) {
		return meshList[_pass];
	}

	//�Ȃ������ꍇ�V�����������ǉ�
	XFILE* lpx = new XFILE();
	lpx->Load(DEV, _pass);
	meshList.insert(std::pair<std::string, XFILE*>(_pass, lpx));
	return lpx;
}

CheckMesh & CheckMesh::GetInstance()
{
	if (s_Instance == nullptr) { s_Instance = new CheckMesh(); }
	return *s_Instance;
}

void CheckMesh::DeleteInstance()
{
	if (s_Instance) {
		delete s_Instance;
		s_Instance = nullptr;
	}
}



