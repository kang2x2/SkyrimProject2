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
	_float			m_fDuration = 0.f; // ���� �ð�?

	/* �ִϸ��̼��� �����ؾ� �ϴ� ���� ������ */
	_char					m_szName[MAX_PATH] = "";
	_uint					m_iNumChannel = 0;
	vector<class CChannel*> m_vecChannel;

public:
	static CAnimation* Create(const aiAnimation* _pAiAnimation);
	virtual void Free() override;
};

END