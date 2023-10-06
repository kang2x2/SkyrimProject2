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

		/* �����Ӹ��� ������ �ִ� ������ �ٸ���. 
		   ���� ��� 3�� �����Ӻ��ʹ� Scale ������ ���� ���� �ִ�.
		   ���� ó���� �ϸ鼭 �ʿ��� ������ �־��־�� �Ѵ�. */

		// ���絵 ������ �Ǵ� ���߿� �����ؾ� �� �ڵ��̴�.
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
