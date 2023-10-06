#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initailze(const aiAnimation* _pAiAnimation);

private:
	_float			m_fDuration = 0.f; // 지속 시간?

	/* 애니메이션이 구동해야 하는 뼈의 정보들 */
	_char					m_szName[MAX_PATH] = "";
	_uint					m_iNumChannel = 0;
	vector<class CChannel*> m_vecChannel;

public:
	static CAnimation* Create(const aiAnimation* _pAiAnimation);
	virtual void Free() override;
};

END