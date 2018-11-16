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

	if (MeshList.find(_pass) != MeshList.end()) {
		return MeshList[_pass];
	}

	XFILE* lpx = new XFILE();
	lpx->Load(DEV, _pass);
	MeshList.insert(std::pair<std::string, XFILE*>(_pass, lpx));
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



