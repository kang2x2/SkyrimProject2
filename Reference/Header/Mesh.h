#pragma once

#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer 
{
private:
	CMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const
	{
		return m_iMaterialIndex;
	}

	const _float3* Get_VtxPos() { return m_pPos; }
	vector<_ulong> Get_Indices() { return m_vecIndex; }

public:
	void Update_VI(const _fmatrix& _matPivot);

public:
	virtual HRESULT Initialize_ProtoType(const CModel* _pModel, const aiMesh* _pAIMesh, _fmatrix _matPivot, CModel::MODEL_TYPE _eType);
	virtual HRESULT Initialize_Clone(void* _pArg) override;

public:
	HRESULT Bind_BondMatrices(class CShader* _pShader, vector<class CBone*> _vecBone, const char* _strConstantName);

private:
	_char m_szName[MAX_PATH] = "";
	_uint m_iMaterialIndex = 0;
	_uint m_iNumBones = 0; // �� �޽ø� �����ϴ� �������� �� �� ���� ������ �޴���
	/* �� �޽ô� � ������ ������ �޴���. ������ �ε��� == ��Ŭ������ ��� �ִ� ���迭�� �ε���  */
	vector<_uint> m_vecBoneIndex;
	vector<_float4x4> m_OffsetMatrices;

	// ��ŷ��.
	_float3* m_pPos = nullptr;
	vector<_ulong> m_vecIndex;

private:
	HRESULT Ready_VertexBuffer_For_NonAnim(const aiMesh* _pAIMesh, _fmatrix _matPivot);
	HRESULT Ready_VertexBuffer_For_Anim(const CModel* _pModel, const aiMesh* _pAIMesh);

public:
	static CMesh* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const CModel* _pModel, const aiMesh* _pAIMesh, _fmatrix _matPivot, CModel::MODEL_TYPE _eType);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

