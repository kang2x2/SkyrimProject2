#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* _pAIChannel);

private:
	_char				m_szName[MAX_PATH] = ""; /* 뼈의 이름 */
	_uint				m_iNumKeyFrames = 0; /* 시간에 따른 상태값 보관 */
	vector<KEYFRAME>	m_vecKeyFrame; /* keyframe : 특정 시간에 표현해야 할 뼈의 상태 */

public:
	static CChannel* Create(const aiNodeAnim* _pAIChannel);
	virtual void Free() override;
};

END

