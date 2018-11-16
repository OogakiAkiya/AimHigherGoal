//==========================================================================================================
// 
// CMTRand.h
//
// ����ҁF���c
//
//==========================================================================================================

#ifndef CMTRand_h
#define CMTRand_h

namespace SimpleLib{

//=================================================================
//
// �����Z���k��c�C�X�^�[�����N���X
//
//=================================================================
class CMTRand{
public:
	CMTRand(){}
	CMTRand(ULONG seed) : m_Rnd(seed){
	}

	// �����V�[�h�ݒ�
	void InitSeed(ULONG seed){
		m_Rnd.seed(seed);
	}

	// �����̗����擾
	UINT GetInt(){return m_Rnd();}
	// �����̗����擾(0 �` num-1)
	int GetInt(int num){
		return m_Rnd()%num;
	}

	// st �` ed�̊Ԃ́A�����������擾
	int GetInt(int st, int ed){
		// ed�̕����������Ƃ��́Ast�Ɠ���ւ���
		if(ed < st)std::swap(ed,st);

		return (int)GetInt(ed-st+1)+st;
	}

	// st �`�@ed�̊Ԃ́A���������_�̗����擾
	float GetFloat(float st, float ed){
		// ed�̕����������Ƃ��́Ast�Ɠ���ւ���
		if(ed < st)std::swap(ed,st);

		return (GetInt()/4294967295.0f)*(ed-st)+st;
	}

private:
	// STL�̃����Z���k�E�c�C�X�^�[����
	std::mt19937	m_Rnd;
};

}

#endif
