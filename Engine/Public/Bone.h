#pragma once

#include "Base.h"

/*
	1. aiScene		: ���� ���¸� ������ ����.
	2. aiBone		: ������ �޽� �������� �ٽ� �籸���Ͽ� ������ �ִ�.
					  �� ���� �� �޽��� � �������� �� �ۼ�Ʈ�� ������ �ִ����� ���� ������ ������ �ִ�.
	3. aiNodel		: ������ ��Ӱ��踦 ǥ���ϱ� ���� �����͸� ������ �ִ�.
					  ������ ��������� ������ �ִ�.
	4. aiAnimNode : 
*/

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	HRESULT Initialize(const aiNode* _pAINode, _int _iParentBoneIndex);
	HRESULT Update_CombinedTransformationMatrix(const vector<class CBone*>& _vecBone);

private:
	char	m_szName[MAX_PATH] = ""; // ���� �̸�

	_float4x4		m_TransformationMatrix; // �ڱ� �ڽŸ��� �� ���(�θ� ��������)
	_float4x4		m_CombinedTransformationMatrix; // �θ��� �� ��İ� ���� �� ���.

	_int			m_iParentBoneIndex = -1; // �θ� ���� �ε���(������ -1)

public:
	static CBone* Create(const aiNode* _pAINode, _int _iParentBoneIndex);
	virtual void Free() override;
};

END

