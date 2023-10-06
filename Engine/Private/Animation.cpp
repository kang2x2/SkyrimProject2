#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initailze(const aiAnimation* _pAiAnimation)
{
	strcpy_s(m_szName, _pAiAnimation->mName.data);

	/* aiAnimation의 druration이 double 타입. float으로 형변환 */
	m_fDuration = (_float)_pAiAnimation->mDuration; 

	/* 애니메이션이 사용하는 뼈의 개수 */
	m_iNumChannel = _pAiAnimation->mNumChannels;

	for (size_t i = 0; i < m_iNumChannel; ++i)
	{
		CChannel* pChannel = CChannel::Create(_pAiAnimation->mChannels[i]);
		if (pChannel == nullptr)
			return E_FAIL;

		m_vecChannel.push_back(pChannel);
	}

	return S_OK;
}

CAnimation* CAnimation::Create(const aiAnimation* _pAiAnimation)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initailze(_pAiAnimation)))
	{
		MSG_BOX("Fail Create : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	__super::Free();
}
