#ifndef CheckMesh_h		//ModelShader_h�͊e�w�b�_�[���Ƃɕς��Ȃ��ƃG���[���o��
#define CheckMesh_h


class CheckMesh 
{
public:
	//---------------------------------------------------------
	// ���擾
	//---------------------------------------------------------
	XFILE* Get(std::string _pass);

	//---------------------------------------------------------
	// �V���O���g��
	//---------------------------------------------------------
	static CheckMesh & GetInstance();
	static void DeleteInstance();

private:
	CheckMesh();
	~CheckMesh();

	//---------------------------------------------------------
	// �ϐ�
	//---------------------------------------------------------
	std::map<std::string, XFILE*> meshList;

	//---------------------------------------------------------
	// �V���O���g��
	//---------------------------------------------------------
	static CheckMesh* s_Instance;
	CheckMesh & operator=(CheckMesh &);
	CheckMesh(CheckMesh&);
};
#define CHECKMESH CheckMesh::GetInstance()


#endif		//�C���N���[�h�K�[�h�I��