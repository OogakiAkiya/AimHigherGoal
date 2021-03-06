#include"../StandardLibraryInclude.h"
#include"../DirectX/D3D.h"
#include"XFILE.h"
#include"CheckMesh.h"

CheckMesh* CheckMesh::s_Instance = nullptr;

CheckMesh::CheckMesh() {

}

CheckMesh::~CheckMesh(){
}

std::shared_ptr<XFILE> CheckMesh::Get(std::string _pass) {

	//リスト内にあるか探索
	if (meshList.find(_pass) != meshList.end()) {
		return meshList[_pass];
	}

	//なかった場合新しく生成し追加
	std::shared_ptr<XFILE> lpx = std::make_shared<XFILE>();
	lpx->Load(DEV, _pass);
	meshList.insert(std::pair<std::string, std::shared_ptr<XFILE>>(_pass, lpx));
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



