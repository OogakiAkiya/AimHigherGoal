#ifndef __KeyFrameAnimatoin_h__
#define __KeyFrameAnimatoin_h__

namespace SimpleLib {

//=====================================================
//! @brief キーフレームアニメ用クォータニオンデータ
//!
//! @ingroup Animation
//=====================================================
struct CAnimeKey_Quaternion {
	float Time;			//!< キーの位置(時間)
	CQuat Value;		//!< キーのクォータニオン値

	// 
	CAnimeKey_Quaternion() : Time(0){
	}

	//! @brief 指定時間の回転データを補間計算で求める(static関数)
	//! @param[in] rotateList		… キーフレーム配列
	//! @param[in] time				… 補間計算で求めたい時間を指定
	//! @param[out]	outQ			… 補間結果の回転(クォータニオン)が入る
	//! @param[inout] nowKeyIndex	… キーフレーム配列のどの位置からキーの検索を行うか？さらに検索後はそのIndexが入る。nullptr指定で最初から検索(処理効率低)
	static bool InterpolationFromTime(const std::vector<CAnimeKey_Quaternion*>& keyList, double time, CQuat &outQ, UINT* nowKeyIndex = nullptr);
};

//=====================================================
//! @brief キーフレームアニメ用ベクトルデータ
//!
//! @ingroup Animation
//=====================================================
struct CAnimeKey_Vector3 {
	float Time;			//!< キーの位置
	CVector3 Value;		//!< キーのベクトル値

	// 
	CAnimeKey_Vector3() : Time(0), Value(0, 0, 0){
	}

	//! @brief 指定時間の回転データを補間計算で求める(static関数)
	//! @param[in] rotateList		… キーフレーム配列
	//! @param[in] time				… 補間計算で求めたい時間を指定
	//! @param[out]	outQ			… 補間結果の回転(クォータニオン)が入る
	//! @param[inout] nowKeyIndex	… キーフレーム配列のどの位置からキーの検索を行うか？さらに検索後はそのIndexが入る。nullptr指定で最初から検索(処理効率低)
	static bool InterpolationFromTime(const std::vector<CAnimeKey_Vector3*>& keyList, double time, CVector3 &outV, UINT* nowKeyIndex = nullptr);
};

//==========================================================
//! @brief 複数の回転・拡大・座標を持ったアニメーションデータ
//!
//! @ingroup Animation
//==========================================================
class CKeyframeAnime {
public:
	std::vector<CAnimeKey_Quaternion*>	m_Rotate;	//!< 回転キーリスト
	std::vector<CAnimeKey_Vector3*>		m_Scale;	//!< 拡大キーリスト
	std::vector<CAnimeKey_Vector3*>		m_Pos;		//!< 座標キーリスト

	//! @brief 解放
	void Release();

	//
	CKeyframeAnime(){
	}
	//
	~CKeyframeAnime() {
		Release();
	}
};

//==========================================================
//!
//! @brief １種類のアニメーションのデータ
//!
//! 複数の行列(ボーンなど)をアニメーション可能	\n
//!
//! @ingroup Animation
//==========================================================
class CAnimationSet {
public:
	std::string						m_AnimeName;			//!< アニメ名
	double							m_TicksPerSecond;		//!< アニメの実行速度
	double							m_AnimeLen;				//!< アニメの長さ
	std::vector<CKeyframeAnime*>	m_FrameAnimeList;		//!< 全フレームぶんのキーフレームデ－タ

	//! @brief 解放
	void Release();

	//! @brief m_FrameAnimeListを指定sizeの個数分確保
	void CreateFrameAnimeList(UINT size);

	// 
	CAnimationSet() : m_TicksPerSecond(60), m_AnimeLen(0){
	}
	// 
	~CAnimationSet() {
		Release();
	}


};


//=====================================================================
//!
//! @brief アニメーショントラック情報クラス
//!
//! アニメーションの進行を記憶するための物
//!
//! @ingroup Animation
//=====================================================================
class CAnimatorTrack {
public:
	//=====================================================================
	// トラックデータ
	//=====================================================================
	SPtr<CAnimationSet>		m_pSkinAnime;	//!< アニメデータへのアドレス
	int						m_AnimeNo;		//!< 現在選択中のアニメ番号
	float					m_Weight;		//!< 重み	
	double					m_Speed;		//!< 速度
	double					m_AnimePos;		//!< アニメ位置	
	bool					m_Loop;			//!< ループフラグ
	bool					m_Enable;		//!< 有効フラグ
	bool					m_DeleteTrack;	//!< 削除フラグ trueでトラック削除


	//=====================================================================
	// トラックイベントノード用クラス
	//=====================================================================
	//! @brief トラックイベント基本クラス
	class Event_Base {
	public:
		//! @brief イベントの種類
		enum SKINTRACKEVENT {
			STE_ENABLE,			//!< 有効無効イベント
			STE_POSITION,		//!< アニメ位置イベント
			STE_SPEED,			//!< アニメ速度イベント
			STE_WEIGHT,			//!< 重みイベント
			STE_DELETETRACK,	//!< トラック削除イベント
		};

		UINT Type;			//!< イベントの種類
		double fStartTime;	//!< 開始時間
	};

	//! @brief  Enableイベント
	class Event_Enable : public Event_Base {
	public:
		bool bl;			// Enableイベント用
	};
	
	//! @brief  Deleteイベント
	class Event_DeleteTrack : public Event_Base {
	public:
	};
	
	//! @brief  Potisionイベント
	class Event_Position : public Event_Base {
	public:
		double Pos;
	};
	
	//! @brief  Speedイベント
	class Event_Speed : public Event_Base {
	public:
		double fDuration;	// 変化時間
		double NewSpeed;
		double OldSpeed;

		char startflag;
		Event_Speed() :startflag(0) {}
	};
	
	//! @brief  ウェイトイベント
	class Event_Weight : public Event_Base {
	public:
		double fDuration;	// 変化時間
		float NewWeight;
		float OldWeight;

		char startflag;
		Event_Weight() :startflag(0) {}
	};

	//=====================================================================
	//! @brief 初期化
	//=====================================================================
	void Init() {
		m_pSkinAnime = nullptr;
		m_AnimeNo = 0;
		m_Weight = 0;
		m_Speed = 0;
		m_AnimePos = 0;
		m_Enable = false;
		m_Loop = false;
		m_DeleteTrack = false;
	}

	//--------------------------------------------------------------------------
	//! @brief Event進行
	//--------------------------------------------------------------------------
	void UpdateEvent(double val);

	//=====================================================================
	// イベント系関数
	//=====================================================================
	void EventTrackEnable(bool NewEnable, double StartTime) {
		UPtr<Event_Enable> lpAdd = UPtr<Event_Enable>(new Event_Enable());

		lpAdd->Type = Event_Base::STE_ENABLE;
		lpAdd->bl = NewEnable;
		lpAdd->fStartTime = StartTime;
		EventList.push_back(std::move(lpAdd));
	}
	void EventTrackDelete(double StartTime) {
		UPtr<Event_DeleteTrack> lpAdd = UPtr<Event_DeleteTrack>(new Event_DeleteTrack());

		lpAdd->Type = Event_Base::STE_DELETETRACK;
		lpAdd->fStartTime = StartTime;
		EventList.push_back(std::move(lpAdd));
	}
	void EventTrackPosition(double NewPos, double StartTime) {
		UPtr<Event_Position> lpAdd = UPtr<Event_Position>(new Event_Position());

		lpAdd->Type = Event_Base::STE_POSITION;
		lpAdd->Pos = NewPos;
		lpAdd->fStartTime = StartTime;
		EventList.push_back(std::move(lpAdd));
	}
	void EventTrackSpeed(double NewSpeed, double StartTime, double Duration) {
		UPtr<Event_Speed> lpAdd = UPtr<Event_Speed>(new Event_Speed());

		lpAdd->Type = Event_Base::STE_SPEED;
		lpAdd->NewSpeed = NewSpeed;
		lpAdd->OldSpeed = 0;
		lpAdd->fStartTime = StartTime;
		lpAdd->fDuration = Duration;
		EventList.push_back(std::move(lpAdd));
	}
	void EventTrackWeight(float NewWeight, double StartTime, double Duration) {
		UPtr<Event_Weight> lpAdd = UPtr<Event_Weight>(new Event_Weight());

		lpAdd->Type = Event_Base::STE_WEIGHT;
		lpAdd->NewWeight = NewWeight;
		lpAdd->OldWeight = 0;
		lpAdd->fStartTime = StartTime;
		lpAdd->fDuration = Duration;
		EventList.push_back(std::move(lpAdd));
	}

	// イベントをリストから全て消す
	void UnkeyTrackEvents() {
		std::list<UPtr<Event_Base>>::iterator it = EventList.begin();
		while(it != EventList.end()) {
			it = EventList.erase(it);
		}
	}

	// 
	CAnimatorTrack() {
		Init();
	}
	~CAnimatorTrack(){
		UnkeyTrackEvents();
	}

private:

	std::list<UPtr<Event_Base>> EventList;	//!< イベントリスト

};

//=====================================================================
//!
//! @brief アニメーション管理クラス
//! 
//!  アニメーションの再生全般を担い、登録している行列に反映する							\n
//!  m_Trackに稼働しているアニメーションの情報が入る									\n
//!  基本はm_Track[0]が現在のアニメとなる。アニメ合成中は[1]以降も作成される			\n
//!  (ChangeAnimeSmooth関数など)														\n
//!
//! @ingroup Animation
//=====================================================================
class CAnimator {
public:
	//=====================================================================
	//
	// 初期設定系
	//
	//=====================================================================

	//-----------------------------------------------------------
	//! @brief 初期化
	//-----------------------------------------------------------
	void Init();

	//-----------------------------------------------------------
	//! @brief 初期化し、srcAnimatorの内容と同じようにする
	//!  アニメリストと行列参照テーブルを同じに構築する。トラック情報はコピーされない。
	//-----------------------------------------------------------
	void Init(CAnimator& srcAnimator);

	//-----------------------------------------------------------
	//! @brief 初期化し、行列参照テーブルにrefMatを登録する
	//! さらに行列を追加したい場合はAddRefMatrix関数で。
	//-----------------------------------------------------------
	void Init(CMatrix* refMat);


	//=====================================================================
	//
	// アニメーション結果を適用する、行列参照テーブル設定
	//
	//=====================================================================

	//-----------------------------------------------------------
	//! @brief アニメーション結果を適用する行列テーブルを削除する
	//-----------------------------------------------------------
	void ClearRefMatrix() {
		m_RefMatrixTbl.clear();
		m_RefMatrixTbl.shrink_to_fit();
	}

	//-----------------------------------------------------------
	//! @brief アニメーション結果を適用する行列の参照を１つ追加登録する
	//! @param[inout] mat				… アニメーション進行時に更新される行列のアドレス
	//! @param[in] isBlendTrack_Rota	… 複数トラック稼働時、回転成分をブレンドするか？
	//! @param[in] isBlendTrack_Scale	… 複数トラック稼働時、拡大成分をブレンドするか？
	//! @param[in] isBlendTrack_Pos		… 複数トラック稼働時、座標成分をブレンドするか？
	//-----------------------------------------------------------
	void AddRefMatrix(CMatrix* mat,
		bool isBlendTrack_Rota = true,
		bool isBlendTrack_Scale = true,
		bool isBlendTrack_Pos = true
	) {
		RefMatrixData data;
		data.refTransMat = mat;
		data.isBlendTrack_Rota = isBlendTrack_Rota;
		data.isBlendTrack_Scale = isBlendTrack_Scale;
		data.isBlendTrack_Pos = isBlendTrack_Pos;
		m_RefMatrixTbl.push_back(data);
	}

	//-----------------------------------------------------------
	//! @brief アニメーション結果を適用する行列の参照を変更する
	//! @param[in] idx					… 登録Indexを指定
	//! @param[inout] mat				… アニメーション進行時に更新される行列のアドレス
	//! @param[in] isBlendTrack_Rota	… 複数トラック稼働時、回転成分をブレンドするか？
	//! @param[in] isBlendTrack_Scale	… 複数トラック稼働時、拡大成分をブレンドするか？
	//! @param[in] isBlendTrack_Pos		… 複数トラック稼働時、座標成分をブレンドするか？
	//-----------------------------------------------------------
	void ChangeRefMatrix(int idx, CMatrix* mat,
		bool isBlendTrack_Rota = true,
		bool isBlendTrack_Scale = true,
		bool isBlendTrack_Pos = true
	) {
		m_RefMatrixTbl[idx].refTransMat = mat;
		m_RefMatrixTbl[idx].isBlendTrack_Rota = isBlendTrack_Rota;
		m_RefMatrixTbl[idx].isBlendTrack_Scale = isBlendTrack_Scale;
		m_RefMatrixTbl[idx].isBlendTrack_Pos = isBlendTrack_Pos;
	}

	//=====================================================================
	//
	// アニメーション登録
	//
	//=====================================================================

	//-----------------------------------------------------------
	//! @brief アニメーションデータ１つを追加登録する
	//! @param[in] allowSameName	… true:無条件で追加する 　false:同名のアニメ名がある場合、そこに上書きされる
	//-----------------------------------------------------------
	void AddAnimation(SPtr<CAnimationSet> anime, bool allowSameName = false);

	void CopyAnimationList(const std::vector<SPtr<CAnimationSet>>& animeTbl) {
		m_AnimeList = animeTbl;
	}

	//=====================================================================
	//
	// アニメーション進行・計算
	//
	//=====================================================================

	//-----------------------------------------------------------
	//! @brief トラック情報取得
	//! 現在再生中のトラックは[0]
	//-----------------------------------------------------------
	std::deque<SPtr<CAnimatorTrack>>& GetTrack() { return m_Track; }

	//-----------------------------------------------------------
	//! @brief 指定トラックのアニメのNo取得
	//! @param[in]	trackNo	… 取得したいトラック番号
	//-----------------------------------------------------------
	int		GetAnimeNo(int trackNo = 0) { return m_Track[trackNo]->m_AnimeNo; }

	//-----------------------------------------------------------
	//! @brief 指定トラックのアニメ速度のNo取得
	//! @param[in]	trackNo	… 取得したいトラック番号
	//-----------------------------------------------------------
	double	GetAnimeSpeed(int trackNo = 0){ return m_Track[trackNo]->m_Speed; }

	//-----------------------------------------------------------
	//! @brief 指定トラックのアニメ位置を取得
	//! @param[in]	trackNo	… 取得したいトラック番号
	//-----------------------------------------------------------
	double	GetAnimePos(int trackNo = 0) { return m_Track[trackNo]->m_AnimePos; }

	//-----------------------------------------------------------
	//! @brief 最大アニメ数取得
	//-----------------------------------------------------------
	int		GetMaxAnimeNum();

	//-----------------------------------------------------------
	//! @brief アニメーションが最後まで行ったか判定
	//! @param[in]	trackNo	… 取得したいトラック番号
	//! @return 最後までいった:true
	//-----------------------------------------------------------
	bool IsAnimationEnd(UINT TrackNo = 0) {
		if(TrackNo >= m_Track.size())return true;
		if(m_Track[TrackNo]->m_Enable == false)return true;
		if(m_Track[TrackNo]->m_pSkinAnime == nullptr)return true;

		if(m_Track[TrackNo]->m_AnimePos >= m_Track[TrackNo]->m_pSkinAnime->m_AnimeLen)return true;
		return false;
	}

	//-----------------------------------------------------------
	//! @brief Valだけアニメーションを進行させ、全ボーンのTransMatを更新する
	//! @param[in]	  Val			… 進行速度
	//! @param[inout] mulDifference0… 0番目に登録されている参照行列の変化差分を、この行列に合成する
	//-----------------------------------------------------------
	void Animation(double Val);

	//-----------------------------------------------------------
	//! @brief アニメーション変更
	//!
	//! ※直接指定したトラックにデータを上書きする										\n
	//!   指定したトラックが存在しなかった場合や存在しないAnimeNoの場合はfalseが返る	\n
	//!
	//! @param[in] AnimeNo		… アニメ番号
	//! @param[in] loop			… ループ再生をする？
	//! @param[in] SetTrackNo	… 情報をセットするトラックの番号
	//! @param[in] bEnableTrack	… そのトラックを有効にする？
	//! @param[in] Speed		… アニメ速度
	//! @param[in] Weight		… 重み
	//! @param[in] AnimePos		… アニメ位置
	//! @return 正常に変更できたらtrue
	//-----------------------------------------------------------
	bool ChangeAnime(UINT AnimeNo, bool loop, UINT SetTrackNo = 0, bool bEnableTrack = true, double Speed = 1.0, float Weight = 1.0f, double AnimePos = 0);

	//-----------------------------------------------------------
	//! @brief ChangeAnime関数の、アニメ名指定バージョン
	//-----------------------------------------------------------
	bool ChangeAnime(const std::string& AnimeName, bool loop, UINT SetTrackNo = 0, bool bEnableTrack = true, double Speed = 1.0, float Weight = 1.0f, double AnimePos = 0);

	//-----------------------------------------------------------
	//! @brief 滑らかにアニメを変更
	//!
	//! ※新しいトラックを作成・設定し、前回のトラックは削除する				\n
	//! ※Durationにより、滑らかにトラック同士のブレンドを行うことができるので、\n
	//!   ゲームのアニメ切り替えで使用すると良い								\n
	//!
	//! @param[in] AnimeNo		… アニメ番号
	//! @param[in] StartTime	… 変化を開始する時間
	//! @param[in] Duration		… 変化にかかる時間 0だと即座に変更
	//! @param[in] loop			… ループ再生をする？
	//! @param[in] Speed		… アニメ速度
	//! @param[in] AnimePos		… アニメ位置
	//! @return 正常に変更できたらtrue
	//-----------------------------------------------------------
	bool ChangeAnimeSmooth(UINT AnimeNo, float StartTime, float Duration, bool loop, double Speed = 1.0, double AnimePos = 0);

	//-----------------------------------------------------------
	//! @brief ChangeAnimeSmooth関数の、アニメ名指定バージョン
	//-----------------------------------------------------------
	bool ChangeAnimeSmooth(const std::string& AnimeName, float StartTime, float Duration, bool loop, double Speed = 1.0, double AnimePos = 0);

	//
	bool AddAnimeSmooth(UINT AnimeNo, float StartTime, float Duration, bool loop, double Speed, double AnimePos, float startWeight, float endWeight);
	//
	bool AddAnimeSmooth(const std::string& AnimeName, float StartTime, float Duration, bool loop, double Speed, double AnimePos, float startWeight, float endWeight);

	//-----------------------------------------------------------
	//! @brief 全トラックのイベントを全て消す
	//-----------------------------------------------------------
	void UnkeyAllTrackEvents() {
		for(auto& var : m_Track){
			var->UnkeyTrackEvents();
		}
	}

	//-----------------------------------------------------------
	//! @brief [0]を除く、全トラックを削除
	//-----------------------------------------------------------
	void ResetTrack(){
		m_Track.clear();
		m_Track.push_back(std::make_shared<CAnimatorTrack>());
		m_Track[0]->Init();
	}

	//-----------------------------------------------------------
	//! @brief アニメーション名からIndexを取得。ない場合は-1
	//! @param[in]	AnimeName	… アニメ名
	//! @return 発見：アニメ番号　アニメ無し：-1
	//-----------------------------------------------------------
	int SearchAnimation(const std::string& AnimeName) {
		for (UINT i = 0; i<m_AnimeList.size(); i++) {
			if (m_AnimeList[i]->m_AnimeName == AnimeName) {
				return i;
			}
		}
		return -1;
	}

	//=====================================================================
	//
	// ベースウェイト
	//
	//=====================================================================

	//-----------------------------------------------------------
	//! @brief アニメータの重みを取得
	//-----------------------------------------------------------
	float GetBaseWeight() { return m_BaseWeight; }

	//-----------------------------------------------------------
	//! @brief アニメータの合成の重みをセット
	//! @param[in] weight	… セットする重み(0～1)
	//! @param[in] speed	… 重みを、現在地からweightへ徐々に変化させる速度　1で即座にセット
	//-----------------------------------------------------------
	void SetBaseWeight(float weight, float speed = 1.0f) {
		m_BaseWeightAnime = weight;
		m_BaseWeightAnimeSpeed = speed;
		if(speed >= 1.0f) {
			m_BaseWeight = weight;
		}
	}



	//=====================================================================
	// 
	CAnimator();

	// 
	~CAnimator() {
	}

private:

	//! @brief トラックの情報からアニメーション補間を行い、全ボーンのTransMat更新
	void CalcAnimation();

	//-----------------------------------------------------------
	// アニメータ自体の合成の重み
	//-----------------------------------------------------------
	float			m_BaseWeight;					//!< 現在全体のベースブレンドウェイト
	float			m_BaseWeightAnime;				//!< 滑らか変化時に使用。変化後のベースブレンドウェイト
	float			m_BaseWeightAnimeSpeed;			//!< 滑らか変化時に使用。変化速度

	//-----------------------------------------------------------
	//! @brief アニメーションリスト
	//-----------------------------------------------------------
	std::vector<SPtr<CAnimationSet>>	m_AnimeList;

	//-----------------------------------------------------------
	//! @brief アニメーショントラック情報
	//!
	//!  ※最低でも１つは存在する	\n
	//!  ※ChangeAnimeSmooth系の関数で新しいTrackが追加される。複数Trackがある場合は、複数アニメが合成される。\n
	//-----------------------------------------------------------
	std::deque<SPtr<CAnimatorTrack>>	m_Track;

	//-----------------------------------------------------------
	// 行列参照構造データ
	//  １つのボーン単位でもある
	//-----------------------------------------------------------
	struct RefMatrixData{

		// 参照行列　ここで参照されている行列へ、アニメーション結果がコピーされる
		CMatrix*	refTransMat = nullptr;
		
		// アニメーション再生 最適化用データ
		UINT		nowKeyIndex_Rota = 0;
		UINT		nowKeyIndex_Scale = 0;
		UINT		nowKeyIndex_Pos = 0;

		bool		isBlendTrack_Rota = true;	// 複数トラック稼働時、回転をブレンドするか？
		bool		isBlendTrack_Scale = true;	// 複数トラック稼働時、拡大をブレンドするか？
		bool		isBlendTrack_Pos = true;	// 複数トラック稼働時、座標をブレンドするか？

	};

	std::vector<RefMatrixData>	m_RefMatrixTbl;

};


}

#endif
