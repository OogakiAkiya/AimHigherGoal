#ifndef Camera_h
#define Camera_h

class Camera {
public:
	Camera(int _flg=ORIGINAL);
	~Camera();
	void ComplianceUpdate();					//追従カメラ
												
	//---------------------------------------------------------
	//情報取得
	//---------------------------------------------------------
	D3DXVECTOR3* GetPos();
	D3DXVECTOR3* GetLook();
	D3DXVECTOR3* GetHead();
	D3DXMATRIX* GetView();
	D3DXMATRIX* GetProj();
	D3DXMATRIX GetRotMat();
	D3DXMATRIX* GetMat();
	void SetMat(D3DXMATRIX* _mat);

	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int ORIGINAL = 1;
	static const int THREE_PERSON_PERSPECTiVE = 2;


private:
	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	float angle;
	D3DXVECTOR3 pos;		//カメラの座標
	D3DXVECTOR3 look;		//カメラの見る位置
	D3DXVECTOR3 head;		//カメラの頭の向き
	D3DXMATRIX mat;			//カメラ行列
	D3DXMATRIX targetmat;	//追従する対象の行列
	D3DXMATRIX view;		//ビュー行列
	D3DXMATRIX proj;		//射影行列

};

#endif