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

	//ƒŠƒXƒg“à‚É‚ ‚é‚©’Tõ
	if (meshList.find(_pass) != meshList.end()) {
		return meshList[_pass];
	}

	//‚È‚©‚Á‚½ê‡V‚µ‚­¶¬‚µ’Ç‰Á
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



