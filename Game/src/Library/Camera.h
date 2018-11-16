#ifndef Camera_h
#define Camera_h

class CharacterBase;

class Camera {
public:
	Camera();
	~Camera();
	void ComplianceUpdate();					//追従カメラ
												
	//情報取得
	D3DXVECTOR3 GetPos();
	D3DXVECTOR3 GetLook();
	D3DXVECTOR3 GetHead();
	D3DXMATRIX GetView();
	D3DXMATRIX GetProj();
	D3DXMATRIX GetRotMat();
	D3DXMATRIX GetMat();
	void SetMat(D3DXMATRIX _mat);

	//void SetTarget(CharacterBase * _tar) { target = _tar; }
private:
	static Camera* s_Instance;
	float angle;
	D3DXVECTOR3 pos;		//カメラの座標
	D3DXVECTOR3 look;	//カメラの見る位置
	D3DXVECTOR3 head;
	D3DXMATRIX mat;		//カメラ行列
	D3DXMATRIX targetmat;
};

#endif