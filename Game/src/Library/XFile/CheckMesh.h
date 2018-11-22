#ifndef CheckMesh_h		//ModelShader_hは各ヘッダーごとに変えないとエラーが出る
#define CheckMesh_h


class CheckMesh 
{
public:
	//---------------------------------------------------------
	// 情報取得
	//---------------------------------------------------------
	XFILE* Get(std::string _pass);

	//---------------------------------------------------------
	// シングルトン
	//---------------------------------------------------------
	static CheckMesh & GetInstance();
	static void DeleteInstance();

private:
	CheckMesh();
	~CheckMesh();

	//---------------------------------------------------------
	// 変数
	//---------------------------------------------------------
	std::map<std::string, XFILE*> meshList;

	//---------------------------------------------------------
	// シングルトン
	//---------------------------------------------------------
	static CheckMesh* s_Instance;
	CheckMesh & operator=(CheckMesh &);
	CheckMesh(CheckMesh&);
};
#define CHECKMESH CheckMesh::GetInstance()


#endif		//インクルードガード終了