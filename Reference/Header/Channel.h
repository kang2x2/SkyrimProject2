#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const class CModel* _pModel, const aiNodeAnim* _pAIChannel);
	void Update_TransformationMatrix(_uint* _pCurKeyFrame, vector<class CBone*>& _vecBone, _float _fTrackPosition);

private:
	_char				m_szName[MAX_PATH] = ""; /* ���� �̸� */

	 /* �ð��� ���� ���°� ���� */
	_uint				m_iNumKeyFrames = 0;
	_uint				m_iBoneIndex = 0;

	vector<KEYFRAME>	m_vecKeyFrame; /* keyframe : Ư�� �ð��� ǥ���ؾ� �� ���� ���� */

public:
	static CChannel* Create(const class CModel* _pModel, const aiNodeAnim* _pAIChannel);
	virtual void Free() override;
};

END

