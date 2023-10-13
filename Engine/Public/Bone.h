#pragma once

#include "Base.h"
#include "Bin_AIScene.h"

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
	CBone(const CBone& rhs);
	virtual ~CBone() = default;

public:
	HRESULT Initialize(const CBin_AIScene::DESC_NODE* _pAINode, _int _iParentBoneIndex);
	HRESULT Update_CombinedTransformationMatrix(const vector<class CBone*>& _vecBone);

public:
	const char* Get_BoneName() const { return m_szName; }
	_float4x4 Get_CombinedTransformationMatrix() const { return m_CombinedTransformationMatrix; }
	void Set_TransformationMatrix(_fmatrix _matTransformation) { XMStoreFloat4x4(&m_TransformationMatrix, _matTransformation); }

private:
	char	m_szName[MAX_PATH] = ""; // ���� �̸�

	_float4x4		m_TransformationMatrix; // �ڱ� �ڽŸ��� �� ���(�θ� ��������)
	_float4x4		m_CombinedTransformationMatrix; // �θ��� �� ��İ� ���� �� ���.

	_int			m_iParentBoneIndex = -1; // �θ� ���� �ε���(������ -1)

public:
	static CBone* Create(const CBin_AIScene::DESC_NODE* _pAINode, _int _iParentBoneIndex);
	CBone* Clone();
	virtual void Free() override;
};

END

