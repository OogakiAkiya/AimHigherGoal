#ifndef CheckMesh_h		//ModelShader_hは各ヘッダーごとに変えないとエラーが出る
#define CheckMesh_h

class XFILE;

class CheckMesh 
{
public:
	CheckMesh();
	~CheckMesh();
	XFILE* Get(std::string _pass);
	// シングルトン
	static CheckMesh & GetInstance();
	static void DeleteInstance();

private:
	static CheckMesh* s_Instance;
	std::map<std::string, XFILE*> MeshList;

};
#define CHECKMESH CheckMesh::GetInstance()


#endif		//インクルードガード終了