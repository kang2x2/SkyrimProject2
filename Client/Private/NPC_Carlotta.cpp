#include "framework.h"
#include "NPC_Carlotta.h"

#include "GameInstance.h"
#include "Bone.h"

#include "Player.h"

#include "Carlotta_Skeleton.h"
#include "Carlotta_Body.h"

CNPC_Carlotta::CNPC_Carlotta(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSkyrim_NPC(_pDevice, _pContext)
{
}

CNPC_Carlotta::CNPC_Carlotta(const CNPC_Carlotta& rhs)
	: CSkyrim_NPC(rhs)
{
}

HRESULT CNPC_Carlotta::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CNPC_Carlotta::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CNPC_Carlotta::Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg)
{
	pMatPivot = (_matrix*)pArg;

	m_strModelComTag = _strModelComTag;

	if (FAILED(Ready_Component(_iLevel)))
		return E_FAIL;

	if (FAILED(Ready_Part()))
		return E_FAIL;

	m_bHasMesh = true;
	m_bCreature = true;
	m_strName = TEXT("NPC_CarlottaValentia");
	m_vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Set_Speed(3.f);

	Play_Animation(true, "animobjectcounteridlearmsloopright");

	if (FAILED(Ready_State()))
		return E_FAIL;

	return S_OK;
}

void CNPC_Carlotta::PriorityTick(_float _fTimeDelta)
{
	for (auto& iter : m_vecNpcPart)
	{
		if (iter != nullptr)
			iter->PriorityTick(_fTimeDelta);
	}
}

void CNPC_Carlotta::Tick(_float _fTimeDelta)
{
	m_pModelCom->Play_Animation(_fTimeDelta);

	for (auto& iter : m_vecNpcPart)
	{
		if (iter != nullptr)
			iter->Tick(_fTimeDelta);
	}

	if (g_curLevel == LEVEL_GAMEPLAY)
		//m_pStateManager->Update(_fTimeDelta);

	__super::Tick(_fTimeDelta);

	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();

	for (size_t i = 0; i < CARLOTTA_COL_END; ++i)
	{
		if (m_pVecCollider[i] != nullptr)
		{
			m_pVecCollider[i]->Update(matWorld);
		}
	}

}

void CNPC_Carlotta::LateTick(_float _fTimeDelta)
{
	if (g_curLevel == LEVEL_GAMEPLAY)
		// m_pStateManager->Late_Update();

	for (auto& iter : m_vecNpcPart)
	{
		if (iter != nullptr)
			iter->LateTick(_fTimeDelta);
	}

#ifdef _DEBUG

	for (auto& collider : m_pVecCollider)
	{
		/* 콜라이더를 그 때도 오리지널을 그리는 게 아니라 행렬을 곱해놓은 aabb를 그린다. */
		if (collider != nullptr)
			m_pRendererCom->Add_Debug(collider);
	}

#endif 

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>
		(pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Player")));

	if (g_curLevel == LEVEL_GAMEPLAY)
	{
		for (auto& collider : m_pVecCollider)
			collider->IsCollision(dynamic_cast<CCollider*>(pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB"))));

		pGameInstance->Collision_AABBTransition(m_pVecCollider[CARLOTTA_COL_AABB], dynamic_cast<CCollider*>(pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB"))));
	}

	Safe_Release(pGameInstance);

}

HRESULT CNPC_Carlotta::Render()
{
	for (auto& iter : m_vecNpcPart)
	{
		if (iter != nullptr)
			iter->Render();
	}

	__super::Bind_ShaderResource();

	// 메시 몇개
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_BondMatrices(m_pShaderCom, i, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	__super::Render();

	return S_OK;
}

HRESULT CNPC_Carlotta::Set_State(CARLOTTA_STATE _eState)
{
	//m_pStateManager->Set_State(_eState);

	return S_OK;
}

void CNPC_Carlotta::Set_WeaponSocket(const char* _strBoneName)
{
	//dynamic_cast<CFalmerOH_Weapon*>(m_vecMonsterPart[PART_WEAPON])->Set_SoketBone(
	//	m_pModelCom->Get_BonePtr(_strBoneName));
}

HRESULT CNPC_Carlotta::Ready_Part()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject* pPart = nullptr;

	/* For. Skeleton */
	CCarlotta_Skeleton::PART_DESC SkeletonPartDesc;
	SkeletonPartDesc.pParent = this;
	SkeletonPartDesc.pParentTransform = m_pTransformCom;

	pPart = pGameInstance->Add_ClonePartObject(TEXT("ProtoType_GameObject_Carlotta_SkeletonPart"), &SkeletonPartDesc);
	if (pPart == nullptr)
		return E_FAIL;
	m_vecNpcPart.push_back(pPart);

	/* For. Body */
	CCarlotta_Body::BODY_DESC BodyPartDesc;
	BodyPartDesc.pParent = this;
	BodyPartDesc.pParentTransform = m_pTransformCom;
	BodyPartDesc.pSocketBone = dynamic_cast<CCarlottaPart_Base*>(m_vecNpcPart[PART_SKELETON])->Get_SocketBonePtr("NPC COM [COM ]");
	BodyPartDesc.matSocketPivot = dynamic_cast<CCarlottaPart_Base*>(m_vecNpcPart[PART_SKELETON])->Get_SocketPivotMatrix();
	
	pPart = pGameInstance->Add_ClonePartObject(TEXT("ProtoType_GameObject_Carlotta_Body"), &BodyPartDesc);
	if (pPart == nullptr)
		return E_FAIL;
	m_vecNpcPart.push_back(pPart);


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CNPC_Carlotta::Ready_Component(_uint _iLevel)
{
	if (FAILED(__super::Add_CloneComponent(_iLevel, m_strModelComTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	__super::Ready_Component();

#pragma region Collider

	m_pVecCollider.resize(CARLOTTA_COL_END);

	/* AABB */
	CBounding_AABB::BOUNDING_AABB_DESC AABBDesc = {};
	AABBDesc.vExtents = _float3(0.5f, 0.7f, 0.5f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), (CComponent**)&m_pVecCollider[CARLOTTA_COL_AABB], &AABBDesc)))
		return E_FAIL;

	m_pVecCollider[CARLOTTA_COL_AABB]->Set_OwnerObj(this);

	/* Dialog */
	CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc = {};
	SphereDesc.fRadius = 4.f;
	SphereDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Collider_Sphere"),
		TEXT("Com_Collider_Detection"), (CComponent**)&m_pVecCollider[CARLOTTA_COL_DIALOG], &SphereDesc)))
		return E_FAIL;

	m_pVecCollider[CARLOTTA_COL_DIALOG]->Set_OwnerObj(this);

	/* Active */
	SphereDesc.fRadius = 4.f;
	SphereDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Collider_Sphere"),
		TEXT("Com_Collider_MissDetection"), (CComponent**)&m_pVecCollider[CARLOTTA_COL_ACTIVE], &SphereDesc)))
		return E_FAIL;

	m_pVecCollider[CARLOTTA_COL_ACTIVE]->Set_OwnerObj(this);

#pragma endregion

	return S_OK;
}

HRESULT CNPC_Carlotta::Ready_State()
{
	m_fRunSpeed = 2.5f;
	m_fWalkSpeed = 1.5f;
	m_iHp = 100;
	m_iAtk = 10;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//m_pStateManager = CStateManager_FalmerOH::Create(this,
	//	pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Player")),
	//	m_pTransformCom, m_pNavigationCom, m_pVecCollider);

	Safe_Release(pGameInstance);

	return S_OK;
}

CNPC_Carlotta* CNPC_Carlotta::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CNPC_Carlotta* pInstance = new CNPC_Carlotta(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CNPC_Carlotta");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNPC_Carlotta::Clone(void* _pArg)
{
	CNPC_Carlotta* pInstance = new CNPC_Carlotta(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CNPC_Carlotta");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNPC_Carlotta::Clone(_uint _iLevel, const wstring& _strModelComTag, void* _pArg)
{
	CNPC_Carlotta* pInstance = new CNPC_Carlotta(*this);

	if (FAILED(pInstance->Initialize_Clone(_iLevel, _strModelComTag, _pArg)))
	{
		MSG_BOX("Fail Clone : CNPC_Carlotta");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Carlotta::Free()
{
	__super::Free();

	for (auto& iter : m_vecNpcPart)
	{
		if (iter != nullptr)
			Safe_Release(iter);
	}

	for (auto& iter : m_pVecCollider)
		Safe_Release(iter);

	//Safe_Release(m_pStateManager);

}
