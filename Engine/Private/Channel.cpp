#include "Channel.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* _pAIChannel)
{
	strcpy_s(m_szName, _pAIChannel->mNodeName.data);

	m_iNumKeyFrames = max(_pAIChannel->mNumScalingKeys, _pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, _pAIChannel->mNumPositionKeys);

	_float3 vScale;
	_float4 vRotation;
	_float3 vTranslation;

	for (size_t i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME tKeyFrame;
		ZeroMemory(&tKeyFrame, sizeof(tKeyFrame));

		/* 프레임마다 가지고 있는 정보가 다르다. 
		   예를 들어 3번 프레임부터는 Scale 정보가 없을 수도 있다.
		   예외 처리를 하면서 필요한 정보만 넣어주어야 한다. */

		// 현재도 문제가 되는 나중에 수정해야 할 코드이다.
		// if (_pAIChannel->mNumScalingKeys > i)
		// {
		// 	vScale = _pAIChannel->mScalingKeys[i];
		// 	tKeyFrame.fTime = _pAIChannel->mScalingKeys[i].mTime;
		// }
		// if (_pAIChannel->mNumRotationKeys > i)
		// {
		// 	vRotation = _pAIChannel->mRotationKeys[i];
		// 	tKeyFrame.fTime = _pAIChannel->mRotationKeys[i].mTime;
		// }
		// if (_pAIChannel->mNumPositionKeys > i)
		// {
		// 	vTranslation = _pAIChannel->mPositionKeys[i];
		// 	tKeyFrame.fTime = _pAIChannel->mPositionKeys[i].mTime;
		// }
		// 
		// tKeyFrame.vScale = vScale;
		// tKeyFrame.vRotation = vRotation;
		// tKeyFrame.vTranslation = vTranslation;
		// 
		// m_vecKeyFrame.push_back(tKeyFrame);
	}

	return S_OK;
}

CChannel* CChannel::Create(const aiNodeAnim* _pAIChannel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(_pAIChannel)))
	{
		MSG_BOX("Fail Create : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	__super::Free();
}
