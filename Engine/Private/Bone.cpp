#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const aiNode* _pAINode, _int _iParentBoneIndex)
{
	m_iParentBoneIndex = _iParentBoneIndex;

	strcpy_s(m_szName, _pAINode->mName.data);

	memcpy(&m_TransformationMatrix, &_pAINode->mTransformation, sizeof(_float4x4));

	// ����� Col��ķ� �Ǿ��ֱ⿡ Raw��ķ� ��ġ ���־�� ��.
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	return S_OK;
}

HRESULT CBone::Update_CombinedTransformationMatrix(const vector<class CBone*>& _vecBone)
{
	// �ֻ��� �θ��� ��
	if (m_iParentBoneIndex == -1)
	{
		// �ڽ��� m_CombinedTransformationMatrix ����� �ڽ��� �� ����� �ȴ�.
		m_CombinedTransformationMatrix = m_TransformationMatrix;
	}
	// �ֻ��� �θ� �ƴ϶��.
	else
	{
		// �ڽ��� �� ����� ������ �θ� ���� m_CombinedTransformationMatrix�� �Ǿ� �Ѵ�.
		// �ڽ��� ���� �θ� ���� m_CombinedTransformationMatrix�� �����Ͽ�
		// �ڽ��� m_CombinedTransformationMatrix�� ������ ���̴�.
		// �θ��� m_CombinedTransformationMatrix�� �����ϴ� ������ �θ� �ڱ� �θ���
		// m_CombinedTransformationMatrix�� �����Ͽ� �����ϰ� �ֱ� ����.
		// �ֻ��� �θ��� ��쿡�� m_CombinedTransformationMatrix�� �ڱ� m_TransformationMatrix��
		// ������ �־��� ������ ���� ������ ����.
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) *
			XMLoadFloat4x4(&_vecBone[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
	}


	return S_OK;
}

CBone* CBone::Create(const aiNode* _pAINode, _int _iParentBoneIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(_pAINode, _iParentBoneIndex)))
	{
		MSG_BOX("Fail Create : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBone::Free()
{
	__super::Free();
}
