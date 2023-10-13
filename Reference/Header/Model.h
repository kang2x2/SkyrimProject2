#pragma once

#include "Component.h"
#include "Bin_AIScene.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum MODEL_TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_ProtoType(const char* _strModleFilePath, _fmatrix _matPivot, MODEL_TYPE _eType);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	_uint Get_NumMeshes() const {  return m_iNumMeshes; } // 매시 수 반환
	_int  Get_BoneIndex(const char* _strBoneName) const; // 해당 이름을 가지는 뼈의 인덱스 반환

	class CBone* Get_BonePtr(const char* _strBoneName) const;
	_float4x4 Get_PivotMatrix() const { return m_matPivot; }

public:
	vector<class CMesh*> Get_VecMesh() { return m_vecMesh; }
	void				 Update_VI(const _fmatrix& _matPivot);

public:
	HRESULT SetUp_Animation(_bool _bIsLoop, _uint _iAnimationIndex);
	HRESULT Bind_BondMatrices(class CShader* _pShader, _uint _iMeshIndex, const char* _strConstantName);
	HRESULT Bind_MaterialTexture(class CShader* _pShader, const char* _pConstantName, _uint _iMeshIndex, aiTextureType _eType);
	HRESULT Play_Animation(_float _fTimeDelta);
	HRESULT Render(_uint _iMeshIndex);

private:
	HRESULT Ready_Mesh(MODEL_TYPE _eType);
	HRESULT Ready_Material(const char* _pModelFilePath);
	HRESULT Ready_Bone(const CBin_AIScene::DESC_NODE* _pRootNode, _int _iParentBoneIndex);
	HRESULT Ready_Animation();

private:
	/* Base */
	// Assimp::Importer	m_Importer; // FBX파일을 열어서 읽는다.
	// const aiScene*		m_pAIScene = nullptr; // 읽고 얻은 데이터들을 보관한다.

	const class CBin_AIScene* m_pAIScene = nullptr;

	/* 메시 */
	_uint					m_iNumMeshes = 0; // numMeshes
	vector<class CMesh*>	m_vecMesh; // 모델 하나가 여러개의 메시를 가질 수 있다.(옷, 무기 등등)

	/* 재질 */
	_uint					m_iNumMaterails = 0; // 총 재질 수
	vector<MESH_MATERIAL>	m_vecMaterial; // 재질 정보를 담는 벡터

	/* 초기화 행렬 */
	_float4x4				m_matPivot; // 사전 준비를 위한 행렬.

	/* 뼈 */
	vector<class CBone*>	m_vecBone; // 뼈들을 저장.

	/* 애니메이션 */
	_uint						m_iCurAnimationIndex = 0; /* 현재 선택된 애니메이션 인덱스. */
	_uint						m_iNumAnimation = 0; /* 모델이 가지고 있는 애니메이션 수 */
	vector<class CAnimation*>	m_vecAnimation; /* 모델이 가지고 있는 애니메이션 배열 */

public:
	static CModel* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const char* _strModleFilePath, _fmatrix _matPivot, MODEL_TYPE _eType);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;

};

END

