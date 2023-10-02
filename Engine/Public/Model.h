#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_ProtoType(const char* _strModleFilePath, _fmatrix _matPivot);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	vector<class CMesh*> Get_VecMesh() { return m_vecMesh; }

public:
	HRESULT Bind_MaterialTexture(class CShader* _pShader, const char* _pConstantName, _uint _iMeshIndex, aiTextureType _eType);
	HRESULT Render(_uint iMeshIndex);

private:
	HRESULT Ready_Mesh();
	HRESULT Ready_Material(const char* _pModelFilePath);

private:
	Assimp::Importer m_Importer; // FBX������ ��� �д´�.
	const aiScene* m_pAIScene = nullptr; // �а� ���� �����͵��� �����Ѵ�.

	_uint			m_iNumMeshes = 0; // numMeshes
	vector<class CMesh*> m_vecMesh; // �� �ϳ��� �������� �޽ø� ���� �� �ִ�.(��, ���� ���)

	_uint			m_iNumMaterails = 0; // �� ���� ��
	vector<MESH_MATERIAL> m_vecMaterial; // ���� ������ ��� ����

	_float4x4		m_matPivot; // ���� �غ� ���� ���.

public:
	static CModel* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const char* _strModleFilePath, _fmatrix _matPivot);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;

};

END

