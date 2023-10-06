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
	_char				m_szName[MAX_PATH] = ""; /* ���� �̸� */
	_uint				m_iNumKeyFrames = 0; /* �ð��� ���� ���°� ���� */
	vector<KEYFRAME>	m_vecKeyFrame; /* keyframe : Ư�� �ð��� ǥ���ؾ� �� ���� ���� */

public:
	static CChannel* Create(const aiNodeAnim* _pAIChannel);
	virtual void Free() override;
};

END

