#ifndef CheckMesh_h		//ModelShader_h�͊e�w�b�_�[���Ƃɕς��Ȃ��ƃG���[���o��
#define CheckMesh_h

class XFILE;

class CheckMesh 
{
public:
	XFILE* Get(std::string _pass);
	// �V���O���g��
	static CheckMesh & GetInstance();
	static void DeleteInstance();

private:
	CheckMesh();
	~CheckMesh();

	static CheckMesh* s_Instance;
	std::map<std::string, XFILE*> MeshList;

};
#define CHECKMESH CheckMesh::GetInstance()


#endif		//�C���N���[�h�K�[�h�I��