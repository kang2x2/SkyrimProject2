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
	Assimp::Importer m_Importer; // FBX파일을 열어서 읽는다.
	const aiScene* m_pAIScene = nullptr; // 읽고 얻은 데이터들을 보관한다.

	_uint			m_iNumMeshes = 0; // numMeshes
	vector<class CMesh*> m_vecMesh; // 모델 하나가 여러개의 메시를 가질 수 있다.(옷, 무기 등등)

	_uint			m_iNumMaterails = 0; // 총 재질 수
	vector<MESH_MATERIAL> m_vecMaterial; // 재질 정보를 담는 벡터

	_float4x4		m_matPivot; // 사전 준비를 위한 행렬.

public:
	static CModel* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const char* _strModleFilePath, _fmatrix _matPivot);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;

};

END

