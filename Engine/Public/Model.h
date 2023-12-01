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
	_uint Get_NumMeshes() const { return m_iNumMeshes; } // �Ž� �� ��ȯ
	_int  Get_BoneIndex(const char* _strBoneName) const; // �ش� �̸��� ������ ���� �ε��� ��ȯ

	class CBone* Get_BonePtr(const char* _strBoneName) const;
	_float4x4 Get_PivotMatrix() const { return m_matPivot; }

	void   Set_AnimationStop(_bool _bIsStop);

public:
	vector<class CMesh*> Get_VecMesh() { return m_vecMesh; }
	string				 Get_CurAnimationName();
	_bool				 Get_IsAnimationFin();
	_bool				 Get_CurAnimationIsLoop();
	_uint				 Get_CurFrameIndex();
	void				 Update_VI(const _fmatrix& _matPivot);

public:
	HRESULT SetUp_Animation(_bool _bIsLoop, string _strAnimationName, _uint _iChangeIndex);
	HRESULT Bind_BondMatrices(class CShader* _pShader, _uint _iMeshIndex, const char* _strConstantName);
	HRESULT Bind_MaterialTexture(class CShader* _pShader, const char* _pConstantName, _uint _iMeshIndex, aiTextureType _eType);
	HRESULT Play_Animation(_float _fTimeDelta);
	HRESULT Render(_uint _iMeshIndex);

public:
	HRESULT SwapDesc_Armor(CModel* _pModel);

private:
	HRESULT Ready_Mesh(MODEL_TYPE _eType);
	HRESULT Ready_Material(const char* _pModelFilePath);
	HRESULT Ready_Bone();
	//HRESULT Ready_Bone(const CBin_AIScene::DESC_NODE _pRootNode, _int _iParentBoneIndex);
	HRESULT Ready_Animation();

private:
	/* Base */
	// Assimp::Importer	m_Importer; // FBX������ ��� �д´�.
	// const aiScene*		m_pAIScene = nullptr; // �а� ���� �����͵��� �����Ѵ�.

	class CBin_AIScene* m_pAIScene = nullptr;

	/* �޽� */
	_uint					m_iNumMeshes = 0; // numMeshes
	vector<class CMesh*>	m_vecMesh; // �� �ϳ��� �������� �޽ø� ���� �� �ִ�.(��, ���� ���)

	/* ���� */
	_uint					m_iNumMaterails = 0; // �� ���� ��
	vector<MESH_MATERIAL>	m_vecMaterial; // ���� ������ ��� ����

	/* �ʱ�ȭ ��� */
	_float4x4				m_matPivot; // ���� �غ� ���� ���.

	/* �� */
	vector<class CBone*>	m_vecBone; // ������ ����.

	/* �ִϸ��̼� */
	_bool						m_bIsChanging = false; 
	_bool						m_bIsFindAnimation = false;
	_uint						m_iCurAnimationIndex = 0; /* ���� ���õ� �ִϸ��̼� �ε���. */
	_uint						m_iNextAnimationIndex = 0; /* ���ؾ� �� �ִϸ��̼� �ε���. */
	_uint						m_iNumAnimation = 0; /* ���� ������ �ִ� �ִϸ��̼� �� */
	_uint						m_iChangeIndex = 0; /* �ٲ� �ִϸ��̼��� ���� ������ */
	vector<class CAnimation*>	m_vecAnimation; /* ���� ������ �ִ� �ִϸ��̼� �迭 */

	/* �� ��� */
	const char*					m_strModelPath = "";

public:
	static CModel* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const char* _strModleFilePath, _fmatrix _matPivot, MODEL_TYPE _eType);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;

};

END

