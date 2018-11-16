#include "../SimpleLib.h"

using namespace SimpleLib;


bool CAnimeKey_Quaternion::InterpolationFromTime(const std::vector<CAnimeKey_Quaternion*>& keyList, double time, CQuat &outQ, UINT* nowKeyIndex)
{
	if(keyList.size() == 1) {		// �L�[���P�̂Ƃ�
		// ���̓��e���g�p
		outQ = keyList.front()->Value;
		return true;
	}
	else if(keyList.size() >= 2) {			// �L�[������

		// �ŏ��̃L�[����O�Ȃ�A�擪��Ԃ�
		if(time < keyList[0]->Time) {
			outQ = keyList.front()->Value;
			return true;
		}
		// �Ō�̃L�[���߂��Ă�Ƃ�
		if(time >= keyList.back()->Time) {
			// �Ō�̃f�[�^��Ԃ�
			outQ = keyList.back()->Value;
			return true;
		}

		// �ʒu������
		UINT i = 0;
		if(nowKeyIndex == nullptr){
			for(i = 0; i<keyList.size(); i++) {
				// ��������̃L�[�𔭌�
				if(time < keyList[i]->Time) {
					break;
				}
			}
		}
		else{

			// �O���Index
			UINT idx = *nowKeyIndex;
			if(idx >= keyList.size())idx = keyList.size() - 1;

			i = idx;
			int searchEndIndex = (int)idx - 1;
			if(searchEndIndex < 0)searchEndIndex = keyList.size() - 2;

			while(1) {
				if((int)i >= (int)keyList.size() - 1)i = 0;		// �Ō��1�O�܂ōs���΁A�ŏ��ɖ߂�
				if(i == searchEndIndex)break;

				// �͈͓��H
				if(time >= keyList[i]->Time && time < keyList[i + 1]->Time) {
					break;
				}

				i++;
			}

			*nowKeyIndex = (unsigned int)i;
			i++;
		}


		// �v�Z
		if(i < keyList.size()){
			// �䗦�Z�o
			float sa = (keyList[i]->Time - keyList[i - 1]->Time);
			double f = time - keyList[i - 1]->Time;

			if(f == 0.0){
				outQ = keyList[i - 1]->Value;
			}
			else{

				// ���ʐ��`���
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
	if(keyList.size() == 1) {		// �L�[���P�̂Ƃ�
		// ���̓��e���g�p
		outV.Set( &keyList.front()->Value );
		return true;
	}
	else if(keyList.size() >= 2) {			// �L�[������
		// �ŏ��̃L�[����O�Ȃ�A�擪��Ԃ�
		if (time < keyList[0]->Time) {
			outV.Set(&keyList.front()->Value);
			return true;
		}
		// �Ō�̃L�[���߂��Ă�Ƃ�
		if(time >= keyList.back()->Time) {
			// �Ō�̃f�[�^��Ԃ�
			outV.Set(&keyList.back()->Value);
			return true;
		}



		// �ʒu������
		UINT i = 0;
		if(nowKeyIndex == nullptr){
			for(i = 0; i<keyList.size(); i++) {
				// ��������̃L�[�𔭌�
				if(time < keyList[i]->Time) {
					break;
				}
			}
		}
		else{

			// �O���Index
			UINT idx = *nowKeyIndex;
			if(idx >= keyList.size())idx = keyList.size() - 1;

			i = idx;
			int searchEndIndex = (int)idx - 1;
			if(searchEndIndex < 0)searchEndIndex = keyList.size() - 2;

			while(1) {
				if((int)i >= (int)keyList.size() - 1)i = 0;		// �Ō��1�O�܂ōs���΁A�ŏ��ɖ߂�
				if(i == searchEndIndex)break;

				// �͈͓��H
				if(time >= keyList[i]->Time && time < keyList[i + 1]->Time) {
					break;
				}

				i++;
			}

			*nowKeyIndex = (unsigned int)i;
			i++;
		}

		if(i < keyList.size()){
			// �䗦�Z�o
			float sa = (keyList[i]->Time - keyList[i - 1]->Time);
			double f = time - keyList[i - 1]->Time;
			if(f == 0){
				outV.Set( &keyList[i - 1]->Value );
			}
			else{
				// ���`���
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
	// �C�x���g����
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

			// ���̃C�x���g������
			it = EventList.erase(it);
			continue;
		}
		// DeleteTrack
		else if ((*it)->Type == Event_Base::STE_DELETETRACK) {
			Event_DeleteTrack *p = static_cast<Event_DeleteTrack*>((*it).get());

			m_DeleteTrack = true;

			// ���̃C�x���g������
			it = EventList.erase(it);
			continue;
		}
		// Position
		else if ((*it)->Type == Event_Base::STE_POSITION) {
			Event_Position *p = static_cast<Event_Position*>((*it).get());

			m_AnimePos = p->Pos;

			// ���̃C�x���g������
			it = EventList.erase(it);
			continue;
		}
		// Speed
		else if ((*it)->Type == Event_Base::STE_SPEED) {
			Event_Speed *p = static_cast<Event_Speed*>((*it).get());

			if (p->startflag == 0) {
				p->OldSpeed = m_Speed;// ���݂̑��x���L��
				p->startflag = 1;
			}

			if (p->fDuration == 0) {	// �����ɕύX

				m_Speed = p->NewSpeed;

				// ���̃C�x���g������
				it = EventList.erase(it);
				continue;
			}
			else {					// ���ԕ��
				double f;
				f = (-p->fStartTime) / p->fDuration;
				if (f >= 1.0f) {
					// �Ō�
					m_Speed = p->NewSpeed;

					// ���̃C�x���g������
					it = EventList.erase(it);
					continue;
				}
				else {
					// ���
					m_Speed = (float)(p->NewSpeed*(f)+p->OldSpeed*(1 - f));
				}
			}
		}
		//Weight
		else if ((*it)->Type == Event_Base::STE_WEIGHT) {
			Event_Weight* p = static_cast<Event_Weight*>((*it).get());

			if (p->startflag == 0) {
				p->OldWeight = m_Weight;// ���݂̏d�݂��L��
				p->startflag = 1;
			}

			if (p->fDuration == 0) {	// �����ɕύX

				m_Weight = p->NewWeight;

				// ���̃C�x���g������
				it = EventList.erase(it);
				continue;
			}
			else {					// ���ԕ��
				double f;
				f = (-p->fStartTime) / p->fDuration;
				if (f >= 1.0f) {
					// �Ō�
					m_Weight = p->NewWeight;

					// ���̃C�x���g������
					it = EventList.erase(it);
					continue;
				}
				else {
					// ���
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
	// �����̃A�j��������Ȃ����ւ�
	if (allowSameName == false) {
		int idx = SearchAnimation(anime->m_AnimeName);
		if (idx != -1) {
			// ����@�����ւ�
			m_AnimeList[idx] = anime;
			return;
		}
	}

	// �V�K�ǉ�
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
	// RootMove�@�\
	//====================================================
	// ���݂̃A�j���ʒu�������Ă���
	double aniPos = m_Track.back()->m_AnimePos;

	//====================================================
	// �d�݃A�j������
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
		// �e�g���b�N�̏���
		//====================================================
		for (UINT i = 0; i<m_Track.size(); i++) {
			track = m_Track[i].get();

			//========================
			// �A�j���i�߂�
			//========================
			if (track->m_Enable) {
				CAnimationSet* lpSA = track->m_pSkinAnime.get();

				// �i�߂�
				track->m_AnimePos += Val * track->m_Speed * (lpSA->m_TicksPerSecond / 60.0);

				// �Ō㔻����ɂ���
				if (lpSA->m_AnimeLen <= 0) {	// �A�j������0
					track->m_AnimePos = 0;
				}
				else if (track->m_Loop) {	// ���[�v
	//				while(track->m_AnimePos > lpSA->m_Period) {
	//					track->m_AnimePos -= lpSA->m_Period;
	//				}
					if (track->m_AnimePos >= lpSA->m_AnimeLen) {
						track->m_AnimePos = 0;
					}
				}
				else {				// ���[�v�Ȃ�
					if (track->m_AnimePos > lpSA->m_AnimeLen) {
						track->m_AnimePos = lpSA->m_AnimeLen;
					}
				}
			}

			//========================
			// �C�x���g����
			//========================
			track->UpdateEvent(Val);
			if (track->m_DeleteTrack) {
				// �Ō��1�͏����Ȃ�
				if (m_Track.size() >= 2) {
					m_Track.erase(m_Track.begin() + i);
					i--;
				}
			}
		}

	}

	//====================================================
	// �A�j���[�V�����f�[�^���g���A�ϊ��s��X�V
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

	// �t���[����
	UINT FrameSize = m_RefMatrixTbl.size();

	// �S�t���[������
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
		// �S�g���b�N
		for (UINT tr = 0; tr<m_Track.size(); tr++) {
			CAnimatorTrack* track = m_Track[tr].get();
			if (!track->m_Enable)continue;

			anime = track->m_pSkinAnime.get();
			if (i >= anime->m_FrameAnimeList.size())continue;

			// �A�j���ʒu
			fAnimeTime = track->m_AnimePos;
			if (fAnimeTime < 0)fAnimeTime = 0;


			// ��]
			if(CAnimeKey_Quaternion::InterpolationFromTime(anime->m_FrameAnimeList[i]->m_Rotate, fAnimeTime, qRota, &m_RefMatrixTbl[i].nowKeyIndex_Rota)) {
				// ���� �܂��� �d�݂�1
				if (RotaCnt == 0 || track->m_Weight >= 1.0f) {
					qTotal = qRota;
				}
				// �u�����h
				else {
					if (m_RefMatrixTbl[i].isBlendTrack_Rota) {
						CQuat::Slerp(&qTotal,
							&qTotal,
							&qRota,
							track->m_Weight);		// ���(0.0�`1.0)
					}
				}
				RotaCnt++;
			}
			// �g��
			if(CAnimeKey_Vector3::InterpolationFromTime(anime->m_FrameAnimeList[i]->m_Scale, fAnimeTime, vScale, &m_RefMatrixTbl[i].nowKeyIndex_Scale)) {
				// ���� �܂��� �d�݂�1
				if (ScaleCnt == 0 || track->m_Weight >= 1.0f) {
					sTotal = vScale;
				}
				// �u�����h
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
			// ���W
			if(CAnimeKey_Vector3::InterpolationFromTime(anime->m_FrameAnimeList[i]->m_Pos, fAnimeTime, vPos, &m_RefMatrixTbl[i].nowKeyIndex_Pos)) {
				// ���� �܂��� �d�݂�1
				if (PosCnt == 0 || track->m_Weight >= 1.0f) {
					vTotal = vPos;
				}
				// �u�����h
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

		// �L�[���Ȃ��ꍇ�́A�������Ȃ�
		if (RotaCnt == 0 && ScaleCnt == 0 && PosCnt == 0) continue;

		m.CreateIdentity();

		mOldTransMat = *refMat;	// �v�Z�O�̃��[�J���s����L��

		// �x�[�X�E�F�C�g�ł̃u�����h
		// ��]
		if (RotaCnt > 0) {
			// �x�[�X�E�F�C�g��1�����Ȃ�A�v�Z�O�̍s��ƃu�����h(1�ȏゾ�Ə㏑���ɂȂ�̂ŁA�v�Z���Ȃ��Ă悢)
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
		// �g��
		if (ScaleCnt > 0) {
			// �x�[�X�E�F�C�g��1�����Ȃ�A�v�Z�O�̍s��ƃu�����h(1�ȏゾ�Ə㏑���ɂȂ�̂ŁA�v�Z���Ȃ��Ă悢)
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
		// ���W
		if (PosCnt > 0) {
			// �x�[�X�E�F�C�g��1�����Ȃ�A�v�Z�O�̍s��ƃu�����h(1�ȏゾ�Ə㏑���ɂȂ�̂ŁA�v�Z���Ȃ��Ă悢)
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
	// �A�j������
	int animeNo = SearchAnimation(AnimeName);
	if (animeNo == -1)return false;

	return ChangeAnime(animeNo, loop, SetTrackNo, bEnableTrack, Speed, Weight, AnimePos);
}


// �u�����h�Ŋ��炩�A�j���ύX
bool CAnimator::ChangeAnimeSmooth(UINT AnimeNo, float StartTime, float Duration, bool loop, double Speed, double AnimePos)
{
	// �V�����g���b�N�쐬
	SPtr<CAnimatorTrack> newTrack = std::make_shared<CAnimatorTrack>();
	m_Track.push_front(newTrack);

	// �����ɕύX
	if (Duration == 0) {
		// �V�����A�j����ݒ�
		if (ChangeAnime(AnimeNo, loop, 0, true, Speed, 1, AnimePos) == false)return false;

		// �O�̃A�j���͖����ɂ���B
		if (m_Track.size() >= 2) {
			// �P�O�̃g���b�N
			CAnimatorTrack* prevTrack = m_Track[1].get();
			prevTrack->m_Enable = false;
			prevTrack->m_DeleteTrack = true;
			prevTrack->m_Weight = 0;
		}
	}
	// ���炩�ɕύX
	else {
		// �V�����A�j�����Z�b�g
		if (ChangeAnime(AnimeNo, loop, 0, true, Speed, 0, AnimePos) == false)return false;
		// �V�����A�j���̏d�݂��グ�Ă����ݒ�
		newTrack->EventTrackWeight(1, StartTime, Duration);

		// �O�̃A�j���̏d�݂����炵�A�����ɂ��Ă����B
		if (m_Track.size() >= 2) {
			// �P�O�̃g���b�N
			CAnimatorTrack* prevTrack = m_Track[1].get();

			prevTrack->EventTrackEnable(false, StartTime + Duration);	// �����C�x���g
			prevTrack->EventTrackDelete(StartTime + Duration);			// �폜�C�x���g

			prevTrack->EventTrackSpeed(0, StartTime, Duration);			// ���x�C�x���g
			prevTrack->EventTrackWeight(0.0f, StartTime, Duration);		// �d�݃C�x���g
		}

	}
	return true;
}

bool CAnimator::ChangeAnimeSmooth(const std::string& AnimeName, float StartTime, float Duration, bool loop, double Speed, double AnimePos)
{
	// �A�j������
	int animeNo = SearchAnimation(AnimeName);
	if (animeNo == -1)return false;

	return ChangeAnimeSmooth(animeNo, StartTime, Duration, loop, Speed, AnimePos);
}

bool CAnimator::AddAnimeSmooth(UINT AnimeNo, float StartTime, float Duration, bool loop, double Speed, double AnimePos, float startWeight, float endWeight)
{
	// �V�����g���b�N�쐬
	SPtr<CAnimatorTrack> newTrack = std::make_shared<CAnimatorTrack>();
	m_Track.push_back(newTrack);	// ���ɒǉ�

	// �V�����A�j�����Z�b�g
	if(ChangeAnime(AnimeNo, loop, (int)m_Track.size() - 1, true, Speed, startWeight, AnimePos) == false)return false;

	// �V�����A�j���̏d�݂�ω������Ă����ݒ�
	newTrack->EventTrackWeight(endWeight, StartTime, Duration);

	// endWeight��0�Ȃ�A0�ɂȂ�����g���b�N�������
	if(endWeight == 0){
		newTrack->EventTrackEnable(false, StartTime + Duration);	// �����C�x���g
		newTrack->EventTrackDelete(StartTime + Duration);			// �폜�C�x���g
	}

	return true;
}

bool CAnimator::AddAnimeSmooth(const std::string& AnimeName, float StartTime, float Duration, bool loop, double Speed, double AnimePos, float startWeight, float endWeight)
{
	// �A�j������
	int animeNo = SearchAnimation(AnimeName);
	if(animeNo == -1)return false;

	return AddAnimeSmooth(animeNo, StartTime, Duration, loop, Speed, AnimePos, startWeight, endWeight);
}


