#include "framework.h"
#include "Falmer_UnEquip.h"

#include "GameInstance.h"

#include "StateManager_FalmerUE.h"

#include "Bone.h"
#include "Player.h"

#include "FalmerUE_Weapon.h"

CFalmer_UnEquip::CFalmer_UnEquip(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CMonster(_pDevice, _pContext)
{
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

	__super::Initialize_Clone(pArg);

	if (FAILED(Ready_Component(_iLevel)))
		return E_FAIL;

	if (FAILED(Ready_Part()))
		return E_FAIL;

	m_bHasMesh = true;
	m_bCreature = true;
	m_strName = TEXT("Falmer_UnEquip");
	m_fDissloveTime = 2.5f;

	m_vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Set_Speed(3.f);

	Play_Animation(true, "idlesquatloop");

	if (FAILED(Ready_State()))
		return E_FAIL;

	return S_OK;
}

void CFalmer_UnEquip::PriorityTick(_float _fTimeDelta)
{
	if (!g_bIsPause)
	{
		for (auto& iter : m_vecMonsterPart)
		{
			if (iter != nullptr)
				iter->PriorityTick(_fTimeDelta);
		}
	}
}

void CFalmer_UnEquip::Tick(_float _fTimeDelta)
{
	if (!g_bIsPause)
	{
		m_pModelCom->Play_Animation(_fTimeDelta);

		for (auto& iter : m_vecMonsterPart)
		{
			if (iter != nullptr)
				iter->Tick(_fTimeDelta);
		}

		if (g_curLevel == LEVEL_GAMEPLAY)
			m_pStateManager->Update(_fTimeDelta);

		__super::Tick(_fTimeDelta);

		_matrix matWorld = m_pTransformCom->Get_WorldMatrix();

		for (size_t i = 0; i < FALMERUE_COL_END; ++i)
		{
			if (m_pVecCollider[i] != nullptr)
			{
				m_pVecCollider[i]->Update(matWorld);
			}
		}

		if (g_curLevel != LEVEL_TOOL)
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			if (m_pPlayer->Get_IsAttack())
			{
				if (pGameInstance->Collision_Enter(m_pVecCollider[FALMERUE_COL_AABB],
					dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB")))))
				{
					pGameInstance->Add_CloneObject(g_curLevel, TEXT("Layer_Effect"), TEXT("ProtoType_GameObject_BloodSpot"));
					m_fHp -= m_pPlayer->GetAtk();
				}
			}

			pGameInstance->Collision_Out(m_pVecCollider[FALMERUE_COL_AABB],
				dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB"))));


			for (_int i = 0; i < m_pVecCollider.size(); ++i)
			{
				pGameInstance->Collision_Out(m_pVecCollider[i],
					dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB"))));
			}

			Safe_Release(pGameInstance);
		}

		if (m_bIsDissloving)
		{
			m_fCurDissloveTime += _fTimeDelta;

			if (m_fCurDissloveTime > m_fDissloveTime)
				m_bReadyDead = true;
		}
	}

}

void CFalmer_UnEquip::LateTick(_float _fTimeDelta)
{
	if (!g_bIsPause)
	{
		__super::LateTick(_fTimeDelta);

		if (g_curLevel == LEVEL_GAMEPLAY)
			m_pStateManager->Late_Update(_fTimeDelta);

		for (auto& iter : m_vecMonsterPart)
		{
			if (iter != nullptr)
				iter->LateTick(_fTimeDelta);
		}
	}

#ifdef _DEBUG

	for (auto& collider : m_pVecCollider)
	{
		/* �ݶ��̴��� �� ���� ���������� �׸��� �� �ƴ϶� ����� ���س��� aabb�� �׸���. */
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
	for (auto& iter : m_vecMonsterPart)
	{
		if (iter != nullptr)
			iter->Render();
	}

	__super::Bind_ShaderResource();

	if (m_bIsDissloving)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &m_fCurDissloveTime, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_DissolveTime", &m_fDissloveTime, sizeof(_float))))
			return E_FAIL;
	}

	// �޽� �
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_BondMatrices(m_pShaderCom, i, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (!m_bIsDissloving)
		{
			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;
		}
		else if (m_bIsDissloving)
		{
			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;
		}

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

HRESULT CFalmer_UnEquip::Ready_Part()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	CGameObject* pPart = nullptr;
	
	/* For. FalmerUE_Weapon */
	CFalmerUE_Weapon::WEAPON_DESC WeaponPartDesc;
	WeaponPartDesc.pParent = this;
	WeaponPartDesc.pParentTransform = m_pTransformCom;
	WeaponPartDesc.pSocketBone = m_pModelCom->Get_BonePtr("WEAPON");
	WeaponPartDesc.matSocketPivot = m_pModelCom->Get_PivotMatrix();
	
	pPart = pGameInstance->Add_ClonePartObject(TEXT("ProtoType_GameObject_FalmerUE_Weapon"), &WeaponPartDesc);
	if (pPart == nullptr)
		return E_FAIL;
	m_vecMonsterPart.push_back(pPart);

	Safe_Release(pGameInstance);

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
	AABBDesc.vExtents = _float3(0.8f, 0.7f, 0.8f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), (CComponent**)&m_pVecCollider[FALMERUE_COL_AABB], &AABBDesc)))
		return E_FAIL;

	m_pVecCollider[FALMERUE_COL_AABB]->Set_OwnerObj(this);

	/* DETECTION */
	CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc = {};
	SphereDesc.fRadius = 6.f;
	SphereDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Collider_Sphere"),
		TEXT("Com_Collider_Detection"), (CComponent**)&m_pVecCollider[FALMERUE_COL_DETECTION], &SphereDesc)))
		return E_FAIL;

	m_pVecCollider[FALMERUE_COL_DETECTION]->Set_OwnerObj(this);

	/* MISS DETECTION */
	SphereDesc.fRadius = 9.f;
	SphereDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Collider_Sphere"),
		TEXT("Com_Collider_MissDetection"), (CComponent**)&m_pVecCollider[FALMERUE_COL_MISSDETECTION], &SphereDesc)))
		return E_FAIL;

	m_pVecCollider[FALMERUE_COL_MISSDETECTION]->Set_OwnerObj(this);

	/* RUN ATTACK */
	SphereDesc.fRadius = 2.5f;
	SphereDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Collider_Sphere"),
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
	m_fHp = 100;
	m_iAtk = 10;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pStateManager = CStateManager_FalmerUE::Create(this,
		pGameInstance->Find_CloneObject(g_curLevel, TEXT("Layer_Player"), TEXT("Player")),
		m_pTransformCom, m_pNavigationCom, m_pVecCollider);

	Safe_Release(pGameInstance);

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

	for (auto& iter : m_vecMonsterPart)
	{
		if (iter != nullptr)
			Safe_Release(iter);
	}

	for (auto& iter : m_pVecCollider)
		Safe_Release(iter);

	Safe_Release(m_pStateManager);
}
