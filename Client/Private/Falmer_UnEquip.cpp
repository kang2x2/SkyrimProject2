#include "framework.h"
#include "Falmer_UnEquip.h"

#include "GameInstance.h"

#include "StateManager_FalmerUE.h"

#include "Player.h"

CFalmer_UnEquip::CFalmer_UnEquip(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CMonster(_pDevice, _pContext)
{
	// state manager 만들자. 그리고 로더에 원본 추가하자.
}

CFalmer_UnEquip::CFalmer_UnEquip(const CFalmer_UnEquip& rhs)
	: CMonster(rhs)
{
}

HRESULT CFalmer_UnEquip::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CFalmer_UnEquip::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CFalmer_UnEquip::Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg)
{
	pMatPivot = (_matrix*)pArg;

	m_strModelComTag = _strModelComTag;

	if (FAILED(Ready_Component(_iLevel)))
		return E_FAIL;

	m_bHasMesh = true;
	m_bCreature = true;
	m_strName = TEXT("Falmer_UnEquip");
	m_vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Set_Speed(3.f);

	Play_Animation(true, "idlesquatloop");

	if (FAILED(Ready_State()))
		return E_FAIL;

	return S_OK;
}

void CFalmer_UnEquip::PriorityTick(_float _fTimeDelta)
{
}

void CFalmer_UnEquip::Tick(_float _fTimeDelta)
{
	m_pModelCom->Play_Animation(_fTimeDelta);

	if (g_curLevel == LEVEL_GAMEPLAY)
		m_pStateManager->Update(_fTimeDelta);

	__super::Tick(_fTimeDelta);

	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();

	for (auto& collider : m_pVecCollider)
	{
		if(collider != nullptr)
			collider->Update(matWorld);
	}
}

void CFalmer_UnEquip::LateTick(_float _fTimeDelta)
{
	if (g_curLevel == LEVEL_GAMEPLAY)
		m_pStateManager->Late_Update();

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
		
		pGameInstance->Collision_AABBTransition(m_pVecCollider[FALMERUE_COL_AABB], dynamic_cast<CCollider*>(pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB"))));
	}

	Safe_Release(pGameInstance);
}

HRESULT CFalmer_UnEquip::Render()
{
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

HRESULT CFalmer_UnEquip::Set_State(CFalmer_UnEquip::FALMERUE_STATE _eState)
{
	m_pStateManager->Set_State(_eState);

	return S_OK;
}

HRESULT CFalmer_UnEquip::Ready_Component(_uint _iLevel)
{
	if (FAILED(__super::Add_CloneComponent(_iLevel, m_strModelComTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	__super::Ready_Component();

#pragma region Collider

	m_pVecCollider.resize(FALMERUE_COL_END);

	/* AABB */
	CBounding_AABB::BOUNDING_AABB_DESC AABBDesc = {};
	AABBDesc.vExtents = _float3(0.5f, 0.7f, 0.5f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), (CComponent**)&m_pVecCollider[FALMERUE_COL_AABB], &AABBDesc)))
		return E_FAIL;

	m_pVecCollider[FALMERUE_COL_AABB]->Set_OwnerObj(this);

	// 이거 내일 진지하게 다시 생각해보자.
	/* ATTACK AABB*/
	AABBDesc.vExtents = _float3(0.f, 0.7f, 0.f);
	AABBDesc.vCenter = _float3(AABBDesc.vExtents.x - 0.5f, AABBDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Collider_AABB"),
		TEXT("Com_Collider_AtkOBB"), (CComponent**)&m_pVecCollider[FALMERUE_COL_ATKAABB], &AABBDesc)))
		return E_FAIL;

	m_pVecCollider[FALMERUE_COL_ATKAABB]->Set_OwnerObj(this);

	/* DETECTION */
	CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc = {};
	SphereDesc.fRadius = 6.f;
	SphereDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Collider_Sphere"),
		TEXT("Com_Collider_Detection"), (CComponent**)&m_pVecCollider[FALMERUE_COL_DETECTION], &SphereDesc)))
		return E_FAIL;

	m_pVecCollider[FALMERUE_COL_DETECTION]->Set_OwnerObj(this);

	/* MISS DETECTION */
	SphereDesc.fRadius = 9.f;
	SphereDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Collider_Sphere"),
		TEXT("Com_Collider_MissDetection"), (CComponent**)&m_pVecCollider[FALMERUE_COL_MISSDETECTION], &SphereDesc)))
		return E_FAIL;

	m_pVecCollider[FALMERUE_COL_MISSDETECTION]->Set_OwnerObj(this);

	/* RUN ATTACK */
	SphereDesc.fRadius = 2.5f;
	SphereDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Collider_Sphere"),
		TEXT("Com_Collider_AtkRound"), (CComponent**)&m_pVecCollider[FALMERUE_COL_ATKROUND], &SphereDesc)))
		return E_FAIL;

	m_pVecCollider[FALMERUE_COL_ATKROUND]->Set_OwnerObj(this);


#pragma endregion

	return S_OK;
}

HRESULT CFalmer_UnEquip::Ready_State()
{
	m_fRunSpeed = 2.5f;
	m_fWalkSpeed = 1.5f;
	m_iHp = 100;
	m_iAtk = 10;

	m_pStateManager = CStateManager_FalmerUE::Create(this, m_pTransformCom, m_pNavigationCom, m_pVecCollider);

	return S_OK;
}

CFalmer_UnEquip* CFalmer_UnEquip::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CFalmer_UnEquip* pInstance = new CFalmer_UnEquip(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CFalmer_UnEquip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFalmer_UnEquip::Clone(void* _pArg)
{
	CFalmer_UnEquip* pInstance = new CFalmer_UnEquip(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CFalmer_UnEquip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFalmer_UnEquip::Clone(_uint _iLevel, const wstring& _strModelComTag, void* _pArg)
{
	CFalmer_UnEquip* pInstance = new CFalmer_UnEquip(*this);

	if (FAILED(pInstance->Initialize_Clone(_iLevel, _strModelComTag, _pArg)))
	{
		MSG_BOX("Fail Clone : CFalmer_UnEquip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFalmer_UnEquip::Free()
{
	__super::Free();

	for (auto& iter : m_pVecCollider)
		Safe_Release(iter);

	Safe_Release(m_pStateManager);
}
