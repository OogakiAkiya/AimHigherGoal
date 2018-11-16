#include "../SimpleLib.h"

using namespace SimpleLib;


bool CAnimeKey_Quaternion::InterpolationFromTime(const std::vector<CAnimeKey_Quaternion*>& keyList, double time, CQuat &outQ, UINT* nowKeyIndex)
{
	if(keyList.size() == 1) {		// キーが１つのとき
		// その内容を使用
		outQ = keyList.front()->Value;
		return true;
	}
	else if(keyList.size() >= 2) {			// キーが複数

		// 最初のキーより手前なら、先頭を返す
		if(time < keyList[0]->Time) {
			outQ = keyList.front()->Value;
			return true;
		}
		// 最後のキーを過ぎてるとき
		if(time >= keyList.back()->Time) {
			// 最後のデータを返す
			outQ = keyList.back()->Value;
			return true;
		}

		// 位置を検索
		UINT i = 0;
		if(nowKeyIndex == nullptr){
			for(i = 0; i<keyList.size(); i++) {
				// 自分より上のキーを発見
				if(time < keyList[i]->Time) {
					break;
				}
			}
		}
		else{

			// 前回のIndex
			UINT idx = *nowKeyIndex;
			if(idx >= keyList.size())idx = keyList.size() - 1;

			i = idx;
			int searchEndIndex = (int)idx - 1;
			if(searchEndIndex < 0)searchEndIndex = keyList.size() - 2;

			while(1) {
				if((int)i >= (int)keyList.size() - 1)i = 0;		// 最後の1つ前まで行けば、最初に戻す
				if(i == searchEndIndex)break;

				// 範囲内？
				if(time >= keyList[i]->Time && time < keyList[i + 1]->Time) {
					break;
				}

				i++;
			}

			*nowKeyIndex = (unsigned int)i;
			i++;
		}


		// 計算
		if(i < keyList.size()){
			// 比率算出
			float sa = (keyList[i]->Time - keyList[i - 1]->Time);
			double f = time - keyList[i - 1]->Time;

			if(f == 0.0){
				outQ = keyList[i - 1]->Value;
			}
			else{

				// 球面線形補間
				CQuat::Slerp(&outQ,
					&keyList[i - 1]->Value,
					&keyList[i]->Value,
					(float)(f * (1.0f / sa)));
			}

			return true;
		}

		return false;
	}
	return false;
}

bool CAnimeKey_Vector3::InterpolationFromTime(const std::vector<CAnimeKey_Vector3*>& keyList, double time, CVector3 &outV, UINT* nowKeyIndex)
{
	if(keyList.size() == 1) {		// キーが１つのとき
		// その内容を使用
		outV.Set( &keyList.front()->Value );
		return true;
	}
	else if(keyList.size() >= 2) {			// キーが複数
		// 最初のキーより手前なら、先頭を返す
		if (time < keyList[0]->Time) {
			outV.Set(&keyList.front()->Value);
			return true;
		}
		// 最後のキーを過ぎてるとき
		if(time >= keyList.back()->Time) {
			// 最後のデータを返す
			outV.Set(&keyList.back()->Value);
			return true;
		}



		// 位置を検索
		UINT i = 0;
		if(nowKeyIndex == nullptr){
			for(i = 0; i<keyList.size(); i++) {
				// 自分より上のキーを発見
				if(time < keyList[i]->Time) {
					break;
				}
			}
		}
		else{

			// 前回のIndex
			UINT idx = *nowKeyIndex;
			if(idx >= keyList.size())idx = keyList.size() - 1;

			i = idx;
			int searchEndIndex = (int)idx - 1;
			if(searchEndIndex < 0)searchEndIndex = keyList.size() - 2;

			while(1) {
				if((int)i >= (int)keyList.size() - 1)i = 0;		// 最後の1つ前まで行けば、最初に戻す
				if(i == searchEndIndex)break;

				// 範囲内？
				if(time >= keyList[i]->Time && time < keyList[i + 1]->Time) {
					break;
				}

				i++;
			}

			*nowKeyIndex = (unsigned int)i;
			i++;
		}

		if(i < keyList.size()){
			// 比率算出
			float sa = (keyList[i]->Time - keyList[i - 1]->Time);
			double f = time - keyList[i - 1]->Time;
			if(f == 0){
				outV.Set( &keyList[i - 1]->Value );
			}
			else{
				// 線形補間
				CVector3::Lerp(&outV,
					&keyList[i - 1]->Value,
					&keyList[i]->Value,
					(float)(f * (1.0f / sa)));
			}
			return true;
		}

		return false;
	}
	return false;
}

//=============================================================================
//
// CKeyframeAnime
//
//=============================================================================

void CKeyframeAnime::Release()
{
	for (UINT i = 0; i<m_Rotate.size(); i++) {
		SAFE_DELETE(m_Rotate[i]);
	}
	m_Rotate.clear();
	for (UINT i = 0; i<m_Scale.size(); i++) {
		SAFE_DELETE(m_Scale[i]);
	}
	m_Scale.clear();
	for (UINT i = 0; i<m_Pos.size(); i++) {
		SAFE_DELETE(m_Pos[i]);
	}
	m_Pos.clear();
}

//=============================================================================
//
// CAnimationSet
//
//=============================================================================
void CAnimationSet::Release()
{
	m_AnimeName = "";
	m_TicksPerSecond = 60;
	m_AnimeLen = 0;

	for (UINT i = 0; i<m_FrameAnimeList.size(); i++) {
		SAFE_DELETE(m_FrameAnimeList[i]);
	}
	m_FrameAnimeList.clear();
}

void CAnimationSet::CreateFrameAnimeList(UINT size)
{
	for (UINT i = 0; i<m_FrameAnimeList.size(); i++) {
		SAFE_DELETE(m_FrameAnimeList[i]);
	}
	m_FrameAnimeList.resize(size);

	for (UINT i = 0; i<m_FrameAnimeList.size(); i++) {
		m_FrameAnimeList[i] = new CKeyframeAnime();
	}
}

//====================================================================================================
//
// CAnimatorTrack
//
//====================================================================================================

void CAnimatorTrack::UpdateEvent(double val)
{
	// イベント処理
	auto it = EventList.begin();
	while (it != EventList.end()) {
		(*it)->fStartTime -= val;

		if ((*it)->fStartTime > 0) {
			++it;
			continue;
		}

		// Enable
		if ((*it)->Type == Event_Base::STE_ENABLE) {
			Event_Enable *p = static_cast<Event_Enable*>((*it).get());

			m_Enable = p->bl;

			// このイベントを消す
			it = EventList.erase(it);
			continue;
		}
		// DeleteTrack
		else if ((*it)->Type == Event_Base::STE_DELETETRACK) {
			Event_DeleteTrack *p = static_cast<Event_DeleteTrack*>((*it).get());

			m_DeleteTrack = true;

			// このイベントを消す
			it = EventList.erase(it);
			continue;
		}
		// Position
		else if ((*it)->Type == Event_Base::STE_POSITION) {
			Event_Position *p = static_cast<Event_Position*>((*it).get());

			m_AnimePos = p->Pos;

			// このイベントを消す
			it = EventList.erase(it);
			continue;
		}
		// Speed
		else if ((*it)->Type == Event_Base::STE_SPEED) {
			Event_Speed *p = static_cast<Event_Speed*>((*it).get());

			if (p->startflag == 0) {
				p->OldSpeed = m_Speed;// 現在の速度を記憶
				p->startflag = 1;
			}

			if (p->fDuration == 0) {	// 即座に変更

				m_Speed = p->NewSpeed;

				// このイベントを消す
				it = EventList.erase(it);
				continue;
			}
			else {					// 中間補間
				double f;
				f = (-p->fStartTime) / p->fDuration;
				if (f >= 1.0f) {
					// 最後
					m_Speed = p->NewSpeed;

					// このイベントを消す
					it = EventList.erase(it);
					continue;
				}
				else {
					// 補間
					m_Speed = (float)(p->NewSpeed*(f)+p->OldSpeed*(1 - f));
				}
			}
		}
		//Weight
		else if ((*it)->Type == Event_Base::STE_WEIGHT) {
			Event_Weight* p = static_cast<Event_Weight*>((*it).get());

			if (p->startflag == 0) {
				p->OldWeight = m_Weight;// 現在の重みを記憶
				p->startflag = 1;
			}

			if (p->fDuration == 0) {	// 即座に変更

				m_Weight = p->NewWeight;

				// このイベントを消す
				it = EventList.erase(it);
				continue;
			}
			else {					// 中間補間
				double f;
				f = (-p->fStartTime) / p->fDuration;
				if (f >= 1.0f) {
					// 最後
					m_Weight = p->NewWeight;

					// このイベントを消す
					it = EventList.erase(it);
					continue;
				}
				else {
					// 補間
					m_Weight = (float)(p->NewWeight*(f)+p->OldWeight*(1 - f));
				}
			}
		}

		++it;
	}

}

//====================================================================================================
//
// CAnimator
//
//====================================================================================================

void CAnimator::Init()
{
	m_BaseWeight = 1;
	m_BaseWeightAnime = 1;
	m_BaseWeightAnimeSpeed = 0.1f;
	UnkeyAllTrackEvents();

	m_AnimeList.clear();


	m_Track.clear();
	m_Track.push_back(std::make_shared<CAnimatorTrack>());
	m_Track[0]->Init();

	m_RefMatrixTbl.clear();

}

void CAnimator::Init(CAnimator& srcAnimator)
{
	Init();

	m_AnimeList = srcAnimator.m_AnimeList;

	m_RefMatrixTbl.clear();
	m_RefMatrixTbl.resize(srcAnimator.m_RefMatrixTbl.size());
	for (UINT i = 0; i < m_RefMatrixTbl.size(); i++) {
		m_RefMatrixTbl[i].refTransMat = srcAnimator.m_RefMatrixTbl[i].refTransMat;
	}
}


void CAnimator::Init(CMatrix* refMat)
{
	Init();

	ClearRefMatrix();
	AddRefMatrix(refMat);
}

void CAnimator::AddAnimation(SPtr<CAnimationSet> anime, bool allowSameName) {
	// 同名のアニメがあるなら入れ替え
	if (allowSameName == false) {
		int idx = SearchAnimation(anime->m_AnimeName);
		if (idx != -1) {
			// あり　差し替え
			m_AnimeList[idx] = anime;
			return;
		}
	}

	// 新規追加
	m_AnimeList.push_back(anime);
}

CAnimator::CAnimator()
{
}

int CAnimator::GetMaxAnimeNum()
{
	return (int)m_AnimeList.size();
}

void CAnimator::Animation(double Val)
{
	CAnimatorTrack* track;

	//====================================================
	// RootMove機能
	//====================================================
	// 現在のアニメ位置を憶えておく
	double aniPos = m_Track.back()->m_AnimePos;

	//====================================================
	// 重みアニメ処理
	//====================================================
	if(Val > 0){
		if (m_BaseWeight != m_BaseWeightAnime) {
			if (m_BaseWeight > m_BaseWeightAnime) {
				m_BaseWeight -= (float)( m_BaseWeightAnimeSpeed * Val );
				if (m_BaseWeight < m_BaseWeightAnime) {
					m_BaseWeight = m_BaseWeightAnime;
				}
			}
			else if (m_BaseWeight < m_BaseWeightAnime) {
				m_BaseWeight += (float)( m_BaseWeightAnimeSpeed * Val );
				if (m_BaseWeight > m_BaseWeightAnime) {
					m_BaseWeight = m_BaseWeightAnime;
				}
			}
		}

		//====================================================
		// 各トラックの処理
		//====================================================
		for (UINT i = 0; i<m_Track.size(); i++) {
			track = m_Track[i].get();

			//========================
			// アニメ進める
			//========================
			if (track->m_Enable) {
				CAnimationSet* lpSA = track->m_pSkinAnime.get();

				// 進める
				track->m_AnimePos += Val * track->m_Speed * (lpSA->m_TicksPerSecond / 60.0);

				// 最後判定を先にする
				if (lpSA->m_AnimeLen <= 0) {	// アニメ長が0
					track->m_AnimePos = 0;
				}
				else if (track->m_Loop) {	// ループ
	//				while(track->m_AnimePos > lpSA->m_Period) {
	//					track->m_AnimePos -= lpSA->m_Period;
	//				}
					if (track->m_AnimePos >= lpSA->m_AnimeLen) {
						track->m_AnimePos = 0;
					}
				}
				else {				// ループなし
					if (track->m_AnimePos > lpSA->m_AnimeLen) {
						track->m_AnimePos = lpSA->m_AnimeLen;
					}
				}
			}

			//========================
			// イベント処理
			//========================
			track->UpdateEvent(Val);
			if (track->m_DeleteTrack) {
				// 最後の1個は消さない
				if (m_Track.size() >= 2) {
					m_Track.erase(m_Track.begin() + i);
					i--;
				}
			}
		}

	}

	//====================================================
	// アニメーションデータを使い、変換行列更新
	//====================================================
	CalcAnimation();
}

void CAnimator::CalcAnimation()
{
	UINT i;

	CAnimationSet* anime;
	CMatrix m;
	CQuat qTotal;
	CVector3 vTotal;

	// フレーム数
	UINT FrameSize = m_RefMatrixTbl.size();

	// 全フレーム処理
	double fAnimeTime;
	CMatrix mOldTransMat;
	CQuat qRota;
	CVector3 vScale;
	CVector3 sTotal;
	CVector3 vPos;
	CQuat qTmp;
	CVector3 vTmp;

	for (i = 0; i<FrameSize; i++) {

		CMatrix* refMat = m_RefMatrixTbl[i].refTransMat;
		if (refMat == nullptr)continue;

		int RotaCnt = 0;
		int PosCnt = 0;
		int ScaleCnt = 0;
		// 全トラック
		for (UINT tr = 0; tr<m_Track.size(); tr++) {
			CAnimatorTrack* track = m_Track[tr].get();
			if (!track->m_Enable)continue;

			anime = track->m_pSkinAnime.get();
			if (i >= anime->m_FrameAnimeList.size())continue;

			// アニメ位置
			fAnimeTime = track->m_AnimePos;
			if (fAnimeTime < 0)fAnimeTime = 0;


			// 回転
			if(CAnimeKey_Quaternion::InterpolationFromTime(anime->m_FrameAnimeList[i]->m_Rotate, fAnimeTime, qRota, &m_RefMatrixTbl[i].nowKeyIndex_Rota)) {
				// 初回 または 重みが1
				if (RotaCnt == 0 || track->m_Weight >= 1.0f) {
					qTotal = qRota;
				}
				// ブレンド
				else {
					if (m_RefMatrixTbl[i].isBlendTrack_Rota) {
						CQuat::Slerp(&qTotal,
							&qTotal,
							&qRota,
							track->m_Weight);		// 補間(0.0～1.0)
					}
				}
				RotaCnt++;
			}
			// 拡大
			if(CAnimeKey_Vector3::InterpolationFromTime(anime->m_FrameAnimeList[i]->m_Scale, fAnimeTime, vScale, &m_RefMatrixTbl[i].nowKeyIndex_Scale)) {
				// 初回 または 重みが1
				if (ScaleCnt == 0 || track->m_Weight >= 1.0f) {
					sTotal = vScale;
				}
				// ブレンド
				else {
					if (m_RefMatrixTbl[i].isBlendTrack_Scale) {
						CVector3::Lerp(&sTotal,
							&sTotal,
							&vScale,
							track->m_Weight);
					}
				}
				ScaleCnt++;
			}
			// 座標
			if(CAnimeKey_Vector3::InterpolationFromTime(anime->m_FrameAnimeList[i]->m_Pos, fAnimeTime, vPos, &m_RefMatrixTbl[i].nowKeyIndex_Pos)) {
				// 初回 または 重みが1
				if (PosCnt == 0 || track->m_Weight >= 1.0f) {
					vTotal = vPos;
				}
				// ブレンド
				else {
					if (m_RefMatrixTbl[i].isBlendTrack_Pos) {
						CVector3::Lerp(&vTotal,
							&vTotal,
							&vPos,
							track->m_Weight);
					}
				}
				PosCnt++;
			}

		}

		// キーがない場合は、何もしない
		if (RotaCnt == 0 && ScaleCnt == 0 && PosCnt == 0) continue;

		m.CreateIdentity();

		mOldTransMat = *refMat;	// 計算前のローカル行列を記憶

		// ベースウェイトでのブレンド
		// 回転
		if (RotaCnt > 0) {
			// ベースウェイトが1未満なら、計算前の行列とブレンド(1以上だと上書きになるので、計算しなくてよい)
			if (m_BaseWeight < 1) {
				CMatrix::ToQuaternion(&qTmp, &mOldTransMat);
				if (m_BaseWeight > 0) {
					CQuat::Slerp(&qTotal, &qTmp, &qTotal, m_BaseWeight);
				}
				else {
					qTotal = qTmp;
				}
			}

			qTotal.ToMatrix(&m);
			refMat->SetRotation(&m);
		}
		// 拡大
		if (ScaleCnt > 0) {
			// ベースウェイトが1未満なら、計算前の行列とブレンド(1以上だと上書きになるので、計算しなくてよい)
			if (m_BaseWeight < 1) {
				vTmp.Set(mOldTransMat.GetXScale(), mOldTransMat.GetYScale(), mOldTransMat.GetZScale());

				if (m_BaseWeight > 0) {
					CVector3::Lerp(&sTotal, &vTmp, &sTotal, m_BaseWeight);
				}
				else {
					sTotal = vTmp;
				}
			}

			refMat->NormalizeScale();
			m.CreateScale(&sTotal);
			*refMat = m * (*refMat);
		}
		// 座標
		if (PosCnt > 0) {
			// ベースウェイトが1未満なら、計算前の行列とブレンド(1以上だと上書きになるので、計算しなくてよい)
			if (m_BaseWeight < 1) {
				vTmp.Set(&mOldTransMat.GetPos());

				if (m_BaseWeight > 0) {
					CVector3::Lerp(&vTotal, &vTmp, &vTotal, m_BaseWeight);
				}
				else {
					vTotal = vTmp;
				}
			}
			refMat->SetPos(&vTotal);
		}
	}
}

bool CAnimator::ChangeAnime(UINT AnimeNo, bool loop, UINT SetTrackNo, bool bEnableTrack, double Speed, float Weight, double AnimePos)
{
	if (m_AnimeList.size() <= AnimeNo)return false;

	if (SetTrackNo >= m_Track.size())return false;

	m_Track[SetTrackNo]->m_pSkinAnime = m_AnimeList[AnimeNo];
	m_Track[SetTrackNo]->m_AnimeNo = AnimeNo;
	m_Track[SetTrackNo]->m_AnimePos = AnimePos;
	m_Track[SetTrackNo]->m_Enable = bEnableTrack;
	m_Track[SetTrackNo]->m_Speed = Speed;
	m_Track[SetTrackNo]->m_Weight = Weight;
	m_Track[SetTrackNo]->m_Loop = loop;


	return true;
}
bool CAnimator::ChangeAnime(const std::string& AnimeName, bool loop, UINT SetTrackNo, bool bEnableTrack, double Speed, float Weight, double AnimePos)
{
	// アニメ検索
	int animeNo = SearchAnimation(AnimeName);
	if (animeNo == -1)return false;

	return ChangeAnime(animeNo, loop, SetTrackNo, bEnableTrack, Speed, Weight, AnimePos);
}


// ブレンドで滑らかアニメ変更
bool CAnimator::ChangeAnimeSmooth(UINT AnimeNo, float StartTime, float Duration, bool loop, double Speed, double AnimePos)
{
	// 新しいトラック作成
	SPtr<CAnimatorTrack> newTrack = std::make_shared<CAnimatorTrack>();
	m_Track.push_front(newTrack);

	// 即座に変更
	if (Duration == 0) {
		// 新しいアニメを設定
		if (ChangeAnime(AnimeNo, loop, 0, true, Speed, 1, AnimePos) == false)return false;

		// 前のアニメは無効にする。
		if (m_Track.size() >= 2) {
			// １つ前のトラック
			CAnimatorTrack* prevTrack = m_Track[1].get();
			prevTrack->m_Enable = false;
			prevTrack->m_DeleteTrack = true;
			prevTrack->m_Weight = 0;
		}
	}
	// 滑らかに変更
	else {
		// 新しいアニメをセット
		if (ChangeAnime(AnimeNo, loop, 0, true, Speed, 0, AnimePos) == false)return false;
		// 新しいアニメの重みを上げていく設定
		newTrack->EventTrackWeight(1, StartTime, Duration);

		// 前のアニメの重みを減らし、無効にしていく。
		if (m_Track.size() >= 2) {
			// １つ前のトラック
			CAnimatorTrack* prevTrack = m_Track[1].get();

			prevTrack->EventTrackEnable(false, StartTime + Duration);	// 無効イベント
			prevTrack->EventTrackDelete(StartTime + Duration);			// 削除イベント

			prevTrack->EventTrackSpeed(0, StartTime, Duration);			// 速度イベント
			prevTrack->EventTrackWeight(0.0f, StartTime, Duration);		// 重みイベント
		}

	}
	return true;
}

bool CAnimator::ChangeAnimeSmooth(const std::string& AnimeName, float StartTime, float Duration, bool loop, double Speed, double AnimePos)
{
	// アニメ検索
	int animeNo = SearchAnimation(AnimeName);
	if (animeNo == -1)return false;

	return ChangeAnimeSmooth(animeNo, StartTime, Duration, loop, Speed, AnimePos);
}

bool CAnimator::AddAnimeSmooth(UINT AnimeNo, float StartTime, float Duration, bool loop, double Speed, double AnimePos, float startWeight, float endWeight)
{
	// 新しいトラック作成
	SPtr<CAnimatorTrack> newTrack = std::make_shared<CAnimatorTrack>();
	m_Track.push_back(newTrack);	// 後ろに追加

	// 新しいアニメをセット
	if(ChangeAnime(AnimeNo, loop, (int)m_Track.size() - 1, true, Speed, startWeight, AnimePos) == false)return false;

	// 新しいアニメの重みを変化させていく設定
	newTrack->EventTrackWeight(endWeight, StartTime, Duration);

	// endWeightが0なら、0になったらトラックから消す
	if(endWeight == 0){
		newTrack->EventTrackEnable(false, StartTime + Duration);	// 無効イベント
		newTrack->EventTrackDelete(StartTime + Duration);			// 削除イベント
	}

	return true;
}

bool CAnimator::AddAnimeSmooth(const std::string& AnimeName, float StartTime, float Duration, bool loop, double Speed, double AnimePos, float startWeight, float endWeight)
{
	// アニメ検索
	int animeNo = SearchAnimation(AnimeName);
	if(animeNo == -1)return false;

	return AddAnimeSmooth(animeNo, StartTime, Duration, loop, Speed, AnimePos, startWeight, endWeight);
}


