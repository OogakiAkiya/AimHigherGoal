//==========================================================================================================
// 
// CMTRand.h
//
// 制作者：鎌田
//
//==========================================================================================================

#ifndef CMTRand_h
#define CMTRand_h

namespace SimpleLib{

//=================================================================
//
// メルセンヌ･ツイスター乱数クラス
//
//=================================================================
class CMTRand{
public:
	CMTRand(){}
	CMTRand(ULONG seed) : m_Rnd(seed){
	}

	// 乱数シード設定
	void InitSeed(ULONG seed){
		m_Rnd.seed(seed);
	}

	// 整数の乱数取得
	UINT GetInt(){return m_Rnd();}
	// 整数の乱数取得(0 ～ num-1)
	int GetInt(int num){
		return m_Rnd()%num;
	}

	// st ～ edの間の、整数乱数を取得
	int GetInt(int st, int ed){
		// edの方が小さいときは、stと入れ替える
		if(ed < st)std::swap(ed,st);

		return (int)GetInt(ed-st+1)+st;
	}

	// st ～　edの間の、浮動小数点の乱数取得
	float GetFloat(float st, float ed){
		// edの方が小さいときは、stと入れ替える
		if(ed < st)std::swap(ed,st);

		return (GetInt()/4294967295.0f)*(ed-st)+st;
	}

private:
	// STLのメルセンヌ・ツイスター乱数
	std::mt19937	m_Rnd;
};

}

#endif
