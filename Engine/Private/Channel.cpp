#include "Channel.h"

#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const CModel* _pModel, const CBin_AIScene::DESC_ANIMATIONCHANNEL* _pAIChannel)
{
	strcpy_s(m_szName, _pAIChannel->mNodeName.data);

	m_iBoneIndex = _pModel->Get_BoneIndex(m_szName);

	m_iNumKeyFrames = max(_pAIChannel->mNumScalingKeys, _pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, _pAIChannel->mNumPositionKeys);

	_float3 vScale = { 0.f, 0.f, 0.f };
	_float4 vRotation = { 0.f, 0.f, 0.f, 0.f };
	_float4 vTranslation = { 0.f, 0.f, 0.f, 0.f };

	for (size_t i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME tKeyFrame;
		ZeroMemory(&tKeyFrame, sizeof(tKeyFrame));

		/* �����Ӹ��� ������ �ִ� ������ �ٸ���. 
		   ���� ��� 3�� �����Ӻ��ʹ� Scale ������ ���� ���� �ִ�.
		   ���� ó���� �ϸ鼭 �ʿ��� ������ �־��־�� �Ѵ�. */
		
		if (_pAIChannel->mNumScalingKeys > i)
		{
			memcpy(&vScale, &_pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			tKeyFrame.fTime = _pAIChannel->mScalingKeys[i].mTime;
		}

		if (_pAIChannel->mNumRotationKeys > i)
		{
			/*  tKeyFrame�� Rot������ _float4 Ÿ���� ������ �ݸ�
			 	_pAIChannel�� Rot Value�� ���ʹϾ� Ÿ���� ������.
				memcpy�� �ϸ� �� �ڸ��� �� �±� ������ �ϳ� �ϳ� ���� �־��־�� �Ѵ�. */
			vRotation.x = _pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = _pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = _pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = _pAIChannel->mRotationKeys[i].mValue.w;
			tKeyFrame.fTime = _pAIChannel->mRotationKeys[i].mTime;
		}

		if (_pAIChannel->mNumPositionKeys > i)
		{
			memcpy(&vTranslation, &_pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			/* _pAIChannel�� ��ġ Ű�� ���� vector3������ Ÿ���̱⿡ w�� 1�� ���� �ʱ�ȭ �ؾ� �Ѵ�. */
			vTranslation.w = 1.f;
			tKeyFrame.fTime = _pAIChannel->mPositionKeys[i].mTime;
		}

		tKeyFrame.vScale = vScale;
		tKeyFrame.vRotation = vRotation;
		tKeyFrame.vTranslation = vTranslation;

		m_vecKeyFrame.push_back(tKeyFrame);
	}

	return S_OK;
}

void CChannel::Update_TransformationMatrix(_uint* _pCurKeyFrame, vector<class CBone*>& _vecBone, _float _fTrackPosition)
{
	/* �ִϸ��̼� �ѹ� ������ �����. */
	if (_fTrackPosition == 0.f)
		*_pCurKeyFrame = 0;

	_float3 vScale;
	_float4 vRotation;
	_float4 vTranslation;

	KEYFRAME tLastKeyFrame = m_vecKeyFrame.back();

	/* ���� Ʈ���� ������ Ű �������� �Ѿ�� ������ ���� ���¸� ����. */
	if (_fTrackPosition >= tLastKeyFrame.fTime)
	{
		*_pCurKeyFrame = m_vecKeyFrame.size() - 1;
		vScale = tLastKeyFrame.vScale;
		vRotation = tLastKeyFrame.vRotation;
		vTranslation = _float4(tLastKeyFrame.vTranslation.x, tLastKeyFrame.vTranslation.y, tLastKeyFrame.vTranslation.z, 1.f );
	}
	else /* �װ� �ƴ϶�� ���� �����Ӱ� ���� �����Ӱ��� ���� ������ �ǽ��Ѵ�. 
		    �� �ִϸ��̼� ������ �����Ӱ��� ������ ����.*/
	{
		/* Ʈ�� ��ġ�� ���� �������� ���� �ð��� �����ϸ� ���� ���������� ���� */
		while (_fTrackPosition >= m_vecKeyFrame[*_pCurKeyFrame + 1].fTime)
			++*_pCurKeyFrame;

		/* ����? ���? ���ϱ� */
		/* ���� Ʈ�� ��ġ - ���� ������ ���� �ð� / ���� ������ ���� �ð� - ���� ������ ���� �ð�*/
		_float fRatio = (_fTrackPosition - m_vecKeyFrame[*_pCurKeyFrame].fTime) /
		(m_vecKeyFrame[*_pCurKeyFrame + 1].fTime - m_vecKeyFrame[*_pCurKeyFrame].fTime);

		/* ������ */
		_vector vSourScale = XMLoadFloat3(&m_vecKeyFrame[*_pCurKeyFrame].vScale);
		_vector vDestScale = XMLoadFloat3(&m_vecKeyFrame[*_pCurKeyFrame + 1].vScale);
		XMStoreFloat3(&vScale, XMVectorLerp(vSourScale, vDestScale, fRatio));

		/* ȸ�� */
		_vector vSourRot = XMLoadFloat4(&m_vecKeyFrame[*_pCurKeyFrame].vRotation);
		_vector vDestRot = XMLoadFloat4(&m_vecKeyFrame[*_pCurKeyFrame + 1].vRotation);
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(vSourRot, vDestRot, fRatio));

		/* ��ġ */
		_vector vSourTranslate = XMLoadFloat4(&m_vecKeyFrame[*_pCurKeyFrame].vTranslation);
		_vector vDestTranslate = XMLoadFloat4(&m_vecKeyFrame[*_pCurKeyFrame + 1].vTranslation);
		XMStoreFloat4(&vTranslation, XMVectorLerp(vSourTranslate, vDestTranslate, fRatio));
	}

	/* ���� ��� ���� : ũ * �� * �� ����̴�. */
	/* �߰��� vectorSet�� ��ü�� � ������ �������� ��ȯ�� �� ������ �����Ѵ� .*/
	_matrix matTransformation = XMMatrixAffineTransformation(XMLoadFloat3(&vScale),
		XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMLoadFloat4(&vTranslation));

	/* ���������� ��ȯ�� ����� �� �ִϸ��̼��� �����ϴ� ���� �޽ð� ������ ������ �����Ѵ�. */
	_vecBone[m_iBoneIndex]->Set_TransformationMatrix(matTransformation);

}

_bool CChannel::Change_TransformationMatrix(_uint* _pCurKeyFrame, vector<class CBone*>& _vecBone, _float _fTrackPosition, const KEYFRAME& _destKeyFrame)
{
	_float3 vScale;
	_float4 vRotation;
	_float4 vTranslation;

	if (_fTrackPosition >= 0.2f)
	{
		return true;
	}

	else
	{
		/* ������ */
		_vector vSourScale = XMLoadFloat3(&m_vecKeyFrame[*_pCurKeyFrame].vScale);
		_vector vDestScale = XMLoadFloat3(&_destKeyFrame.vScale);
		XMStoreFloat3(&vScale, XMVectorLerp(vSourScale, vDestScale, 0.2f));

		/* ȸ�� */
		_vector vSourRot = XMLoadFloat4(&m_vecKeyFrame[*_pCurKeyFrame].vRotation);
		_vector vDestRot = XMLoadFloat4(&_destKeyFrame.vRotation);
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(vSourRot, vDestRot, 0.2f));

		/* ��ġ */
		_vector vSourTranslate = XMLoadFloat4(&m_vecKeyFrame[*_pCurKeyFrame].vTranslation);
		_vector vDestTranslate = XMLoadFloat4(&_destKeyFrame.vTranslation);
		XMStoreFloat4(&vTranslation, XMVectorLerp(vSourTranslate, vDestTranslate, 0.2f));

		/* ���� ��� ���� : ũ * �� * �� ����̴�. */
		/* �߰��� vectorSet�� ��ü�� � ������ �������� ��ȯ�� �� ������ �����Ѵ� .*/
		_matrix matTransformation = XMMatrixAffineTransformation(XMLoadFloat3(&vScale),
			XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMLoadFloat4(&vTranslation));

		/* ���������� ��ȯ�� ����� �� �ִϸ��̼��� �����ϴ� ���� �޽ð� ������ ������ �����Ѵ�. */
		_vecBone[m_iBoneIndex]->Set_TransformationMatrix(matTransformation);
	}

	return false;
}

CChannel* CChannel::Create(const class CModel* _pModel, const CBin_AIScene::DESC_ANIMATIONCHANNEL* _pAIChannel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(_pModel, _pAIChannel)))
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
