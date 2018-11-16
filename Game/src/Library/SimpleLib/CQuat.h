#ifndef CQuat_h
#define CQuat_h

namespace SimpleLib{

//============================================================================
// �N�H�[�^�j�I���N���X
//============================================================================
class CQuat : public D3DXQUATERNION{
public:
	// �R���X�g���N�^
	CQuat(){
		x = 0;
		y = 0;
		z = 0;
		w = 1;
	}
	CQuat(const D3DXQUATERNION &q){
		x = q.x;
		y = q.y;
		z = q.z;
		w = q.w;
	}
	CQuat( const float *f){
		x = f[0];
		y = f[1];
		z = f[2];
		z = f[3];
	}
	CQuat( float X, float Y, float Z, float W ){
		x = X;
		y = Y;
		z = Z;
		w = W;
	}
	// ����
	float Dot(const D3DXQUATERNION *q){
		return D3DXQuaternionDot(this,q);
	}
	static float Dot(const D3DXQUATERNION *q1,const D3DXQUATERNION *q2){
		return D3DXQuaternionDot(q1,q2);
	}
	// �w���֐�
	// ���̃��\�b�h�́A�����N�H�[�^�j�I����P�ʃN�H�[�^�j�I���ɕϊ����܂��B�܂�D3DXQuaternionExp �́A�����N�H�[�^�j�I�����n�������̂Ɖ��肵�Aw �𖳎����Čv�Z���s���܂� (w == 0)�B
	void Exp(const D3DXQUATERNION *q){
		D3DXQuaternionExp(this,q);
	}
	static void Exp(D3DXQUATERNION *qOut,const D3DXQUATERNION *q){
		D3DXQuaternionExp(qOut,q);
	}

	// �P��
	void Identity(){
		D3DXQuaternionIdentity(this);
	}
	static void Identity(D3DXQUATERNION *qOut){
		D3DXQuaternionIdentity(qOut);
	}
	// �N�H�[�^�j�I�����������āA�Đ��K�����܂��B
	void Inverse(){
		D3DXQuaternionInverse(this,this);
	}
	static void Inverse(D3DXQUATERNION *qOut,const D3DXQUATERNION *qIn){
		D3DXQuaternionInverse(qOut,qIn);
	}
	// ���K��
	void Normalize(){
		D3DXQuaternionNormalize(this,this);
	}
	static void Normalize(D3DXQUATERNION *out,const D3DXQUATERNION *qIn){
		D3DXQuaternionNormalize(out,qIn);
	}

	// �N�H�[�^�j�I���̒���
	float Length(){
		return D3DXQuaternionLength(this);
	}
	static float Length(const D3DXQUATERNION *qIn){
		return D3DXQuaternionLength(qIn);
	}
	// ���R�ΐ����v�Z
	void Ln(){
		D3DXQuaternionLn(this,this);
	}
	static void Ln(D3DXQUATERNION *qOut,const D3DXQUATERNION *qIn){
		D3DXQuaternionLn(qOut,qIn);
	}
	// ��
	void Multiply(const D3DXQUATERNION *qIn){
		D3DXQuaternionMultiply(this,this,qIn);
	}
	void Multiply_Local(const D3DXQUATERNION *qIn){
		D3DXQuaternionMultiply(this,qIn,this);
	}
	static void Multiply(D3DXQUATERNION *qOut,const D3DXQUATERNION *qIn1,const D3DXQUATERNION *qIn2){
		D3DXQuaternionMultiply(qOut,qIn1,qIn2);
	}

	// �N�H�[�^�j�I����C�ӎ���]
	void RotateAxis(const D3DXVECTOR3 *vAxis,float Ang){
		CQuat q;
		q.SetRotateAxis( vAxis, Ang );
		Multiply( &q );
	}
	void RotateAxis_Local(const D3DXVECTOR3 *vAxis,float Ang){
		CQuat q;
		q.SetRotateAxis( vAxis, Ang );
		Multiply_Local( &q );
	}
	void SetRotateAxis(const D3DXVECTOR3 *vAxis,float Ang){
		D3DXQuaternionRotationAxis(this,vAxis,ToRadian(Ang));
	}

	// ��]�s��ɕϊ�
	void ToMatrix(D3DXMATRIX *out)
	{
		D3DXMatrixRotationQuaternion(out,this);
	}
	static void ToMatrix(D3DXMATRIX *out,D3DXQUATERNION *qIn)
	{
		D3DXMatrixRotationQuaternion(out,qIn);
	}

	// ���ʐ��`���
	static void Slerp(D3DXQUATERNION *out,const D3DXQUATERNION *q1,const D3DXQUATERNION *q2,float f)
	{
		D3DXQuaternionSlerp(out,q1,q2,f);
	}
	// ���󕽕����
	static void Squad(D3DXQUATERNION *out,const D3DXQUATERNION *qQ1,const D3DXQUATERNION *q1,const D3DXQUATERNION *q2,const D3DXQUATERNION *q3,float f)
	{
		D3DXQuaternionSquad(out,qQ1,q1,q2,q3,f);
	}
	// ���󕽕���Ԃ̐���_��ݒ�
	//��)
	// z ���𒆐S�Ƃ�����]
	//	D3DXQUATERNION Q0 = D3DXQUATERNION(0,  0, 0.707f, -.707f);
	//	D3DXQUATERNION Q1 = D3DXQUATERNION(0,  0, 0.000f, 1.000f);
	//	D3DXQUATERNION Q2 = D3DXQUATERNION(0,  0, 0.707f, 0.707f);
	//	D3DXQUATERNION Q3 = D3DXQUATERNION(0,  0, 1.000f, 0.000f);
	//	D3DXQUATERNION A, B, C, Qt;
	//	FLOAT time = 0.5f;
	//
	//	D3DXQuaternionSquadSetup(&A, &B, &C, &Q0, &Q1, &Q2, &Q3);
	//	D3DXQuaternionSquad(&Qt, &Q1, &A, &B, &C, time);
	static void SquadSetup(D3DXQUATERNION *Aout,D3DXQUATERNION *Bout,D3DXQUATERNION *Cout,const D3DXQUATERNION *qQ0,const D3DXQUATERNION *qQ1,const D3DXQUATERNION *qQ2,const D3DXQUATERNION *qQ3){
		D3DXQuaternionSquadSetup(Aout,Bout,Cout,qQ0,qQ1,qQ2,qQ3);
	}
	// Catmull-Rom�X�v���C�����
	static void CatmullRom(D3DXQUATERNION *out,const D3DXQUATERNION *q0,const D3DXQUATERNION *q1,const D3DXQUATERNION *q2,const D3DXQUATERNION *q3,float f)
	{
		D3DXVec4CatmullRom((D3DXVECTOR4*)out, (D3DXVECTOR4*)q0, (D3DXVECTOR4*)q1, (D3DXVECTOR4*)q2, (D3DXVECTOR4*)q3, f);
		D3DXQuaternionNormalize(out, out);
	}

	// �N�H�[�^�j�I���̉�]���Ɖ�]�p�x���v�Z
	void ToAxisAngle(D3DXVECTOR3 *outAxis,float *outAngle){
		D3DXQuaternionToAxisAngle(this,outAxis,outAngle);
	}
};

}

#endif
