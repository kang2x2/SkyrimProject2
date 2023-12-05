#pragma once

#include "Base.h"
#include "Bin_AIScene.h"

#include "Channel.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initailze(const class CModel* _pModel, const CBin_AIScene::DESC_ANIMATION* _pAiAnimation);
	void Update_TransformationMatrix(vector<class CBone*>& _vecBone, _float _fTimeDelta);
	_bool Change_TransformationMatrix(vector<class CBone*>& _vecBone, const vector<CChannel*>& _destVecChannel, _float _fTimeDelta);
	void ReSet();
	void Ready_ChangeAnimation(_uint _iChangeIndex);

public:
	void  Set_Loop(_bool _bIsLoop) { m_bIsLoop = _bIsLoop; }
	
	_char* Get_AnimationName() { return m_szName; }
	_bool  Get_CurAnimationIsLoop() { return m_bIsLoop; }

	void  Reset_TrackPosition(_float _fInitIndex) { m_fTrackPosition = _fInitIndex; }
	_bool Get_Finish() { return m_bIsFinish; }
	const vector<CChannel*>& Get_Channel() { return m_vecChannel; }
	const vector<_uint>& Get_CurKeyFrame() { return m_vecCurKeyFrame; }
	void  Set_KeyFrame(_uint _iFrame) { m_fTrackPosition = _iFrame; }

	void   Set_AnimationStop(_bool _bIsStop) { m_bIsStop = _bIsStop; }

 private:
	_float			m_fDuration = 0.f; // �� ���̶�� ��������.
	_float			m_fTickPerSecond = 0.f; // ��� �ӵ�?
	_float			m_fTrackPosition = 0.f; // �ִϸ��̼� ���� ��� ��ġ.
	_float			m_fCnageTrackPosition = 0.f; // �ִϸ��̼� �� ��ȯ �� ��� ��ġ.
	_bool			m_bIsLoop = false; // ���� ����(������ ��� �� �ִϸ��̼� �Ǻ�)
	_bool			m_bIsFinish = false; // �ִϸ��̼��� �������� Ȯ��.
	_bool			m_bIsStop = false; // �ִϸ��̼� ��ž.

	/* �ִϸ��̼��� �����ؾ� �ϴ� ���� ������ */
	_char					m_szName[MAX_PATH] = "";
	_uint					m_iNumChannel = 0;
	vector<CChannel*>		m_vecChannel;
	vector<_uint>			m_vecCurKeyFrame; // �� ä���� ������ Ű������. ���� �����Ѵ�.


public:
	static CAnimation* Create(const class CModel* _pModel, const CBin_AIScene::DESC_ANIMATION* _pAiAnimation);
	CAnimation* Clone();
	virtual void Free() override;
};

END