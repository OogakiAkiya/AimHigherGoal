//==========================================================================================================
// 
// CMatrix.h
//
// 制作者：鎌田
//
//==========================================================================================================

#ifndef CMatrix_h
#define CMatrix_h

namespace SimpleLib{

//==========================================================================================================
// 行列操作クラス
//	D3DXMATRIXの機能拡張版
//==========================================================================================================
class CMatrix : public D3DXMATRIX{
public:
	//============================================================
	// メンバ関数
	//============================================================
	CMatrix(){
		D3DXMatrixIdentity(this);
	}
	CMatrix(const D3DXMATRIX &m){
		_11 = m._11;
		_12 = m._12;
		_13 = m._13;
		_14 = m._14;
		_21 = m._21;
		_22 = m._22;
		_23 = m._23;
		_24 = m._24;
		_31 = m._31;
		_32 = m._32;
		_33 = m._33;
		_34 = m._34;
		_41 = m._41;
		_42 = m._42;
		_43 = m._43;
		_44 = m._44;
	}
	CMatrix(FLOAT _11, FLOAT _12, FLOAT _13, FLOAT _14,
			FLOAT _21, FLOAT _22, FLOAT _23, FLOAT _24,
			FLOAT _31, FLOAT _32, FLOAT _33, FLOAT _34,
			FLOAT _41, FLOAT _42, FLOAT _43, FLOAT _44 );

	//============================================================================
	// 情報取得
	//============================================================================
	// 座標をベクトル型で取得
	const CVector3& GetPos() const { return *(CVector3*)&_41; }
	// X軸をベクトル型で取得
	const CVector3& GetXAxis() const { return *(CVector3*)&_11; }
	// Y軸をベクトル型で取得
	const CVector3& GetYAxis() const { return *(CVector3*)&_21; }
	// Z軸をベクトル型で取得
	const CVector3& GetZAxis() const { return *(CVector3*)&_31; }

	// X軸の拡大率を取得
	float GetXScale() const { return GetXAxis().Length(); }
	// Y軸の拡大率を取得
	float GetYScale() const { return GetYAxis().Length(); }
	// Z軸の拡大率を取得
	float GetZScale() const { return GetZAxis().Length(); }


	//========================================================================
	//
	// 行列「合成」系関数･･･行列を作成し、合成していく(相対的変化)
	//
	//========================================================================

	// 逆行列作成
	void Inverse()
	{
		D3DXMatrixInverse(this, nullptr, this);
	}
	void Inverse(D3DXMATRIX *destMat)
	{
		D3DXMatrixInverse(destMat, nullptr, this);
	}
	static void Inverse(D3DXMATRIX *mOut, float *pDeterminant, const D3DXMATRIX *mIn) {
		D3DXMatrixInverse(mOut, pDeterminant, mIn);
	}

	// 移動行列を「後ろから」合成
	void Move(float x,float y,float z)
	{
		_41 += x;
		_42 += y;
		_43 += z;
	}
	void Move(const D3DXVECTOR3 *v)
	{
		_41 += v->x;
		_42 += v->y;
		_43 += v->z;
	}
	// 移動行列を「手前から」合成
	void Move_Local(float x,float y,float z)
	{
		D3DXMATRIX TmpMat;		// 作業用
		D3DXMatrixTranslation(&TmpMat,x,y,z);
		D3DXMatrixMultiply(this,&TmpMat,this);// 合成
	}
	void Move_Local(const D3DXVECTOR3 *v)						// 移動を合成。ローカル座標系での移動
	{
		D3DXMATRIX TmpMat;		// 作業用
		D3DXMatrixTranslation(&TmpMat,v->x,v->y,v->z);
		D3DXMatrixMultiply(this,&TmpMat,this);// 合成
	}
	// 回転行列を「後ろから」合成
	void RotateX(float Ang)
	{
		D3DXMATRIX TmpMat;		// 作業用
		D3DXMatrixRotationX(&TmpMat,ToRadian(Ang));
		D3DXMatrixMultiply(this,this,&TmpMat);// 合成
	}
	void RotateY(float Ang)
	{
		D3DXMATRIX TmpMat;		// 作業用
		D3DXMatrixRotationY(&TmpMat,ToRadian(Ang));
		D3DXMatrixMultiply(this,this,&TmpMat);// 合成
	}
	void RotateZ(float Ang)										// Z回転を合成
	{
		D3DXMATRIX TmpMat;		// 作業用
		D3DXMatrixRotationZ(&TmpMat,ToRadian(Ang));
		D3DXMatrixMultiply(this,this,&TmpMat);// 合成
	}
	void RotateAxis(D3DXVECTOR3 *vAxis,float Ang)				// 指定軸回転を合成
	{
		D3DXMATRIX TmpMat;		// 作業用
		D3DXMatrixRotationAxis(&TmpMat,vAxis,ToRadian(Ang));
		D3DXMatrixMultiply(this,this,&TmpMat);// 合成
	}

	// 回転行列を「手前から」合成
	void RotateX_Local(float Ang)								// X回転を合成 ローカル座標系での回転
	{
		D3DXMATRIX TmpMat;		// 作業用
		D3DXMatrixRotationX(&TmpMat,ToRadian(Ang));
		D3DXMatrixMultiply(this,&TmpMat,this);// 合成
	}
	void RotateY_Local(float Ang)								// Y回転を合成 ローカル座標系での回転
	{
		D3DXMATRIX TmpMat;		// 作業用
		D3DXMatrixRotationY(&TmpMat,ToRadian(Ang));
		D3DXMatrixMultiply(this,&TmpMat,this);// 合成
	}
	void RotateZ_Local(float Ang)								// Z回転を合成 ローカル座標系での回転
	{
		D3DXMATRIX TmpMat;		// 作業用
		D3DXMatrixRotationZ(&TmpMat,ToRadian(Ang));
		D3DXMatrixMultiply(this,&TmpMat,this);// 合成
	}
	void RotateAxis_Local(D3DXVECTOR3 *vAxis,float Ang)			// 指定軸回転を合成 ローカル座標系での回転
	{
		D3DXMATRIX TmpMat;		// 作業用
		D3DXMatrixRotationAxis(&TmpMat,vAxis,ToRadian(Ang));
		D3DXMatrixMultiply(this,&TmpMat,this);// 合成
	}
	// スケーリング行列を「後ろから」合成
	void Scale(float x,float y,float z)							// スケーリングを合成
	{
		D3DXMATRIX TmpMat;		// 作業用
		D3DXMatrixScaling(&TmpMat,x,y,z);
		D3DXMatrixMultiply(this,this,&TmpMat);// 合成
	}
	void Scale(const D3DXVECTOR3 *v)
	{															// スケーリングを合成
		D3DXMATRIX TmpMat;		// 作業用
		D3DXMatrixScaling(&TmpMat,v->x,v->y,v->z);
		D3DXMatrixMultiply(this,this,&TmpMat);// 合成
	}
	// スケーリング行列を「手前から」合成
	void Scale_Local(float x,float y,float z)					// スケーリングを合成 ローカル座標系での拡大
	{
		D3DXMATRIX TmpMat;		// 作業用
		D3DXMatrixScaling(&TmpMat,x,y,z);
		D3DXMatrixMultiply(this,&TmpMat,this);// 合成
	}
	void Scale_Local(const D3DXVECTOR3 *v)					// スケーリングを合成 ローカル座標系での拡大
	{
		D3DXMATRIX TmpMat;		// 作業用
		D3DXMatrixScaling(&TmpMat,v->x,v->y,v->z);
		D3DXMatrixMultiply(this,&TmpMat,this);// 合成
	}

	//========================================================================
	//
	// 行列「作成・操作」系関数･･･行列の内容を直接上書きなどの操作をする関数(絶対的変化)
	//
	//========================================================================
	// 単位行列作成
	void CreateIdentity()
	{
		D3DXMatrixIdentity(this);
	}
	void CreateIdentityRotate()
	{
		_11 = 1;_12 = 0;_13=0;_14 = 0;
		_21 = 0;_22 = 1;_23=0;_24 = 0;
		_31 = 0;_32 = 0;_33=1;_34 = 0;
	}
	static void CreateIdentity(D3DXMATRIX *m){
		D3DXMatrixIdentity(m);
	}

	// 移動行列作成
	void CreateMove(float x,float y,float z)
	{
		D3DXMatrixTranslation(this,x,y,z);
	}
	void CreateMove(const D3DXVECTOR3 *v)
	{
		D3DXMatrixTranslation(this,v->x,v->y,v->z);
	}
	static void CreateMove(D3DXMATRIX *mOut,float x,float y,float z)
	{
		D3DXMatrixTranslation(mOut,x,y,z);
	}
	static void CreateMove(D3DXMATRIX *mOut,const D3DXVECTOR3 *v)
	{
		D3DXMatrixTranslation(mOut,v->x,v->y,v->z);
	}
	// X回転行列作成
	void CreateRotateX(float Ang)
	{
		D3DXMatrixRotationX(this,ToRadian(Ang));
	}
	static void CreateRotateX(D3DXMATRIX *mOut,float Ang)
	{
		D3DXMatrixRotationX(mOut,ToRadian(Ang));
	}
	// Y回転行列作成
	void CreateRotateY(float Ang)
	{
		D3DXMatrixRotationY(this,ToRadian(Ang));
	}
	static void CreateRotateY(D3DXMATRIX *mOut,float Ang)
	{
		D3DXMatrixRotationY(mOut,ToRadian(Ang));
	}
	// Z回転行列作成
	void CreateRotateZ(float Ang)
	{
		D3DXMatrixRotationZ(this,ToRadian(Ang));
	}
	static void CreateRotateZ(D3DXMATRIX *mOut,float Ang)
	{
		D3DXMatrixRotationZ(mOut,ToRadian(Ang));
	}
	// 任意軸回転行列を作成
	void CreateRotateAxis(const D3DXVECTOR3 *vAxis,float Ang)
	{
		D3DXMatrixRotationAxis(this,vAxis,ToRadian(Ang));
	}
	static void CreateRotateAxis(D3DXMATRIX *mOut,const D3DXVECTOR3 *vAxis,float Ang)
	{
		D3DXMatrixRotationAxis(mOut,vAxis,ToRadian(Ang));
	}
	// スケーリング行列作成
	void CreateScale(float x,float y,float z)						
	{
		D3DXMatrixScaling(this,x,y,z);
	}
	void CreateScale(const D3DXVECTOR3 *v)
	{
		D3DXMatrixScaling(this,v->x,v->y,v->z);
	}
	static void CreateScale(D3DXMATRIX *mOut,float x,float y,float z)
	{
		D3DXMatrixScaling(mOut,x,y,z);
	}
	static void CreateScale(D3DXMATRIX *mOut,const D3DXVECTOR3 *v)
	{
		D3DXMatrixScaling(mOut,v->x,v->y,v->z);
	}
	// 指定方向にZ軸を向けた行列を作成
	void SetLookAt(const D3DXVECTOR3 *LookWay, const D3DXVECTOR3 *Up);			// LookWayの方向に向けた行列にする。Upは上となる方向を指定。※合成でなく、直接回転部分を上書きします。
	void SetLookAtPos(const D3DXVECTOR3 *TargetPos, const D3DXVECTOR3 *Up);		// 自分からTargetの位置を見た方向の行列にする。Upは上となる方向を指定。※合成でなく、直接回転部分を上書きします。

	// 座標設定
	void SetPos(float fx,float fy,float fz){
		_41 = fx;
		_42 = fy;
		_43 = fz;
	}
	void SetPos(const D3DXVECTOR3 *v){
		_41 = v->x;
		_42 = v->y;
		_43 = v->z;
	}

	// 回転だけコピー
	void SetRotation(const CMatrix *lpSrcMat) {
		_11 = lpSrcMat->_11;
		_12 = lpSrcMat->_12;
		_13 = lpSrcMat->_13;
		_21 = lpSrcMat->_21;
		_22 = lpSrcMat->_22;
		_23 = lpSrcMat->_23;
		_31 = lpSrcMat->_31;
		_32 = lpSrcMat->_32;
		_33 = lpSrcMat->_33;
	}

	// 拡大率を正規化(1）する
	void NormalizeScale()
	{
		D3DXVECTOR3 v;
		v.x = _11; v.y = _12; v.z = _13;
		D3DXVec3Normalize(&v, &v);
		_11 = v.x; _12 = v.y; _13 = v.z;

		v.x = _21; v.y = _22; v.z = _23;
		D3DXVec3Normalize(&v, &v);
		_21 = v.x; _22 = v.y; _23 = v.z;

		v.x = _31; v.y = _32; v.z = _33;
		D3DXVec3Normalize(&v, &v);
		_31 = v.x; _32 = v.y; _33 = v.z;
	}


	//========================================================================
	// 変換
	//========================================================================
	void ToQuaternion(D3DXQUATERNION *out){									// 行列からクォータニオンへ
		D3DXQuaternionRotationMatrix(out,this);
	}
	static void ToQuaternion(D3DXQUATERNION *out,const D3DXMATRIX *mIn){		// 行列からクォータニオンへ
		D3DXQuaternionRotationMatrix(out,mIn);
	}

};

}

#endif
