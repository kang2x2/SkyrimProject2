#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initailze(const class CModel* _pModel, const aiAnimation* _pAiAnimation);
	void Update_TransformationMatrix(vector<class CBone*>& _vecBone, _float _fTimeDelta);
	void ReSet();
public:
	void Set_Loop(_bool _bIsLoop) { m_bIsLoop = _bIsLoop; }

private:
	_float			m_fDuration = 0.f; // �� ���̶�� ��������.
	_float			m_fTickPerSecond = 0.f; // ��� �ӵ�?
	_float			m_fTrackPosition = 0.f; // �ִϸ��̼� ���� ��� ��ġ.
	_bool			m_bIsLoop = false; // ���� ����(������ ��� �� �ִϸ��̼� �Ǻ�)
	_bool			m_bIsFinish = false; // �ִϸ��̼��� �������� Ȯ��.

	/* �ִϸ��̼��� �����ؾ� �ϴ� ���� ������ */
	_char					m_szName[MAX_PATH] = "";
	_uint					m_iNumChannel = 0;
	vector<class CChannel*> m_vecChannel;
	vector<_uint>			m_vecCurKeyFrame; // �� ä���� ������ Ű������. ���� �����Ѵ�.


public:
	static CAnimation* Create(const class CModel* _pModel, const aiAnimation* _pAiAnimation);
	CAnimation* Clone();
	virtual void Free() override;
};

END