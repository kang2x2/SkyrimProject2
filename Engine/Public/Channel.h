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
	_char				m_szName[MAX_PATH] = ""; /* 뼈의 이름 */

	 /* 시간에 따른 상태값 보관 */
	_uint				m_iNumKeyFrames = 0;
	_uint				m_iBoneIndex = 0;

	vector<KEYFRAME>	m_vecKeyFrame; /* keyframe : 특정 시간에 표현해야 할 뼈의 상태 */

public:
	static CChannel* Create(const class CModel* _pModel, const aiNodeAnim* _pAIChannel);
	virtual void Free() override;
};

END

