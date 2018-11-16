//==========================================================================================================
// 
// Helper.h
//
// ����ҁF���c
//
//==========================================================================================================

#ifndef Helper_h
#define Helper_h

//=================================================================
//
// ����ƕ֗��ȃ}�N����֐�
//
//=================================================================


#pragma warning(disable:4996)
#include <string>

namespace SimpleLib{

// �x -> ���W�A�� �ϊ�
#define ToRadian(x) ((x)*0.017453f)
// ���W�A�� -> �x �ϊ�
#define ToDegree(x) ((x)*57.3f)

// ���S�ȃ|�C���^�̉�� ���{����define�łȂ��A�e���v���[�g�֐��Ȃǂɂ����ق����ǂ�
#define SAFE_FREE(p)		{if(p){free(p);p=nullptr;}}				// malloc�������̂����S��free����}�N��
#define SAFE_RELEASE(p)		{if(p){p->Release();p=nullptr;}}		// COM�I�u�W�F�N�g�n�����S��Release����}�N��
#define SAFE_DELETE(p)		{if(p){delete p;p=nullptr;}}			// new���ꂽ�ϐ������S�ɉ������}�N��
#define SAFE_DELETE_ARRAY(p){if(p){delete[] p;p=nullptr;}}			// new���ꂽ�z������S�ɉ������}�N��

// �F�֌W�̒Z�k���p
#define XRGB(r,g,b) D3DCOLOR_XRGB(r,g,b)
#define ARGB(a,r,g,b) D3DCOLOR_ARGB(a,r,g,b)
#define RGBA(r,g,b,a) D3DCOLOR_RGBA(r,g,b,a)
#define COLORVALUE(a,r,g,b) D3DCOLOR_COLORVALUE(r,g,b,a)

//======================================================
// �X�}�[�g�|�C���^�Z�k���p
//======================================================
#define SPtr std::shared_ptr
#define UPtr std::unique_ptr

//======================================================
// FPS�v��
//======================================================
DWORD FpsProc();

//======================================================
// ���O�o��
//======================================================
void AddLogFile(char *LogFileName,char *str, bool bData=false);

//=================================================================
// �ψ������b�Z�[�W�{�b�N�X
//=================================================================
inline void DispMsg(HWND hWnd,char *format, ...)
{
	char str[1024];
	va_list argptr;

	va_start(argptr, format);
	vsprintf(str,format, argptr);
	va_end(argptr);

	MessageBox(hWnd,str,"Message",MB_OK);
}

//======================================================
// �L�[����(1�񉟂�)
//======================================================
bool KeyCheck(int keyCode);

//======================================================
// �t�@�C�����Ɗg���q�̊ԂɁA�w�蕶�����}������������𐶐�
//======================================================
inline std::string ConvertExtFileName(const std::string& FileName, const std::string& ext)
{
	std::string extName = FileName;
	int pos = extName.find_last_of(".");
	if (pos == -1)return "";
	std::string tmp = ".";
	tmp += ext;
	extName.insert(pos, tmp);
	return extName;
}


//======================================================
// ���݂̃����_�[�^�[�Q�b�g���擾���A���ƂŖ߂��֗��N���X
//======================================================
class RT_BackUpper {
public:
	// �R���X�g���N�^
	//  ���݂�RT�̏����擾���A�����Ă���
	RT_BackUpper(LPDIRECT3DDEVICE9 lpD3DDev, int rt_Index) {
		m_rt_Index = rt_Index;
		lpD3DDev->GetRenderTarget(rt_Index, &m_rt);	// RT�擾
	}

	// �f�X�g���N�^
	//  �����Ă�����RT���Z�b�g���Ȃ���
	~RT_BackUpper() {
		if (m_rt) {
			LPDIRECT3DDEVICE9 lpD3DDev;
			m_rt->GetDevice(&lpD3DDev);

			lpD3DDev->SetRenderTarget(m_rt_Index, m_rt);

			SAFE_RELEASE(m_rt);
			SAFE_RELEASE(lpD3DDev);
		}
	}

	void Restore() {
	}

private:
	int					m_rt_Index = 0;
	LPDIRECT3DSURFACE9	m_rt = nullptr;
};

//======================================================
// ���݂�Z�o�b�t�@���擾���A���ƂŖ߂��֗��N���X
//======================================================
class ZBuffer_BackUpper {
public:
	// �R���X�g���N�^
	//  ���݂�RT�̏����擾���A�����Ă���
	ZBuffer_BackUpper(LPDIRECT3DDEVICE9 lpD3DDev) {
		lpD3DDev->GetDepthStencilSurface(&m_z);	// RT�擾
	}

	// �f�X�g���N�^
	//  �����Ă�����RT���Z�b�g���Ȃ���
	~ZBuffer_BackUpper() {
		if (m_z) {
			LPDIRECT3DDEVICE9 lpD3DDev;
			m_z->GetDevice(&lpD3DDev);

			lpD3DDev->SetDepthStencilSurface(m_z);

			SAFE_RELEASE(m_z);
			SAFE_RELEASE(lpD3DDev);
		}
	}

private:
	LPDIRECT3DSURFACE9	m_z = nullptr;
};

}


#endif
