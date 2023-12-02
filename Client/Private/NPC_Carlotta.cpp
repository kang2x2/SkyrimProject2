#include "framework.h"
#include "NPC_Carlotta.h"

#include "GameInstance.h"
#include "Bone.h"

#include "Player.h"

#include "Carlotta_Skeleton.h"
#include "Carlotta_Body.h"
#include "Carlotta_Head.h"
#include "Carlotta_Hand.h"
#include "Carlotta_Foot.h"

CNPC_Carlotta::CNPC_Carlotta(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSkyrim_NPC(_pDevice, _pContext)
{
	Initialize_ProtoType();
}

CNPC_Carlotta::CNPC_Carlotta(const CNPC_Carlotta& rhs)
	: CSkyrim_NPC(rhs)
{
}

HRESULT CNPC_Carlotta::Initialize_ProtoType()
{
	m_strName = TEXT("NPC_CarlottaValentia");
	m_strLayerTag = TEXT("ProtoType_NPC");

	return S_OK;
}

HRESULT CNPC_Carlotta::Initialize_Clone(void* pArg)
{
	pMatPivot = (_matrix*)pArg;

	if (FAILED(Ready_Component(LEVEL_GAMEPLAY)))
		return E_FAIL;

	if (FAILED(Ready_Part()))
		return E_FAIL;

	m_bHasMesh = true;
	m_bCreature = true;
	m_strName = TEXT("NPC_CarlottaValentia");
	m_vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Set_Speed(3.f);

	Play_Animation(true, "mt_runforward");

	if (FAILED(Ready_State()))
		return E_FAIL;

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

	/* 피킹할 때 메시가 무조건 있어야 한다. 즉, 바디의 모델을 가지고 있자. */
	if (FAILED(__super::Add_CloneComponent(TEXT("Com_Model"), dynamic_cast<CModel*>(m_vecNpcPart[PART_BODY]->Get_Component(TEXT("Com_Model"))),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pMatPivot->r[3]);
	m_pTransformCom->Set_Speed(3.f);

	m_bHasMesh = true;
	m_bCreature = true;
	m_strName = TEXT("NPC_CarlottaValentia");
	m_vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


	if (FAILED(Ready_State()))
		return E_FAIL;

	Play_Animation(true, "animobjectcounteridleloopleft");

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
	for (auto& iter : m_vecNpcPart)
	{
		if (iter != nullptr)
			iter->Tick(_fTimeDelta);
	}

	if (g_curLevel == LEVEL_GAMEPLAY)
	{
		//m_pStateManager->Update(_fTimeDelta);
	}

	__super::Tick(_fTimeDelta);

	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();

	for (size_t i = 0; i < CARLOTTA_COL_END; ++i)
	{
		if (m_pVecCollider[i] != nullptr)
		{
			m_pVecCollider[i]->Update(matWorld);
		}
	}

	if (g_curLevel != LEVEL_TOOL)
	{
		_vector	vPosition = m_pNavigationCom->Set_OnCell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		_float4 tempPos;
		XMStoreFloat4(&tempPos, vPosition);
		tempPos.y += 0.10f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&tempPos));
	}

}

void CNPC_Carlotta::LateTick(_float _fTimeDelta)
{
	//if (g_curLevel == LEVEL_GAMEPLAY)
	//	// m_pStateManager->Late_Update(_float _fTimeDelta);
	//
	//for (auto& iter : m_vecNpcPart)
	//{
	//	if (iter != nullptr)
	//		iter->LateTick(_fTimeDelta);
	//}

#ifdef _DEBUG

	//for (auto& collider : m_pVecCollider)
	//{
	//	/* 콜라이더를 그 때도 오리지널을 그리는 게 아니라 행렬을 곱해놓은 aabb를 그린다. */
	//	if (collider != nullptr)
	//		m_pRendererCom->Add_Debug(collider);
	//}

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

	__super::Render();

	return S_OK;
}

HRESULT CNPC_Carlotta::Set_State(CARLOTTA_STATE _eState)
{
	//m_pStateManager->Set_State(_eState);

	return S_OK;
}

/* 어차피 모든 파츠들의 프레임, 애니메이션 이름 등등은 모두 같다. */
void CNPC_Carlotta::Play_Animation( _bool _bIsLoop, string _strAnimationName, _uint _iChangeIndex, _bool _bIsReset, _bool _bIsQuickChange)
{
	dynamic_cast<CCarlotta_Body*>(m_vecNpcPart[PART_BODY])->Set_AnimationIndex(_bIsLoop, _strAnimationName, _iChangeIndex, _bIsReset, _bIsQuickChange);
	dynamic_cast<CCarlotta_Head*>(m_vecNpcPart[PART_HEAD])->Set_AnimationIndex(_bIsLoop, _strAnimationName, _iChangeIndex, _bIsReset, _bIsQuickChange);
	dynamic_cast<CCarlotta_Hand*>(m_vecNpcPart[PART_HAND])->Set_AnimationIndex(_bIsLoop, _strAnimationName, _iChangeIndex, _bIsReset, _bIsQuickChange);
	dynamic_cast<CCarlotta_Foot*>(m_vecNpcPart[PART_FOOT])->Set_AnimationIndex(_bIsLoop, _strAnimationName, _iChangeIndex, _bIsReset, _bIsQuickChange);
}

_bool CNPC_Carlotta::Get_IsAnimationFin()
{
	return dynamic_cast<CCarlotta_Body*>(m_vecNpcPart[PART_BODY])->Get_IsAnimationFin();
}

string CNPC_Carlotta::Get_CurAnimationName()
{
	return dynamic_cast<CCarlotta_Body*>(m_vecNpcPart[PART_BODY])->Get_CurAnimationName();
}

_uint CNPC_Carlotta::Get_CurFrameIndex()
{
	return dynamic_cast<CCarlotta_Body*>(m_vecNpcPart[PART_BODY])->Get_CurFrameIndex();
}

HRESULT CNPC_Carlotta::Ready_Part()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	CGameObject* pPart = nullptr;

	/* For. Body */
	CCarlotta_Body::PART_DESC BodyPartDesc;
	BodyPartDesc.pParent = this;
	BodyPartDesc.pParentTransform = m_pTransformCom;
	
	pPart = pGameInstance->Add_ClonePartObject(TEXT("ProtoType_GameObject_Carlotta_BodyPart"), &BodyPartDesc);
	if (pPart == nullptr)
		return E_FAIL;
	m_vecNpcPart.push_back(pPart);
	
	///* For. Head */
	CCarlotta_Head::PART_DESC HeadPartDesc;
	HeadPartDesc.pParent = this;
	HeadPartDesc.pParentTransform = m_pTransformCom;
	
	pPart = pGameInstance->Add_ClonePartObject(TEXT("ProtoType_GameObject_Carlotta_HeadPart"), &HeadPartDesc);
	if (pPart == nullptr)
		return E_FAIL;
	m_vecNpcPart.push_back(pPart);
	
	/* For. Hand */
	CCarlotta_Hand::PART_DESC HandPartDesc;
	HandPartDesc.pParent = this;
	HandPartDesc.pParentTransform = m_pTransformCom;
	
	pPart = pGameInstance->Add_ClonePartObject(TEXT("ProtoType_GameObject_Carlotta_HandPart"), &HandPartDesc);
	if (pPart == nullptr)
		return E_FAIL;
	m_vecNpcPart.push_back(pPart);
	
	/* For. Foot */
	CCarlotta_Foot::PART_DESC FootPartDesc;
	FootPartDesc.pParent = this;
	FootPartDesc.pParentTransform = m_pTransformCom;
	
	pPart = pGameInstance->Add_ClonePartObject(TEXT("ProtoType_GameObject_Carlotta_FootPart"), &FootPartDesc);
	if (pPart == nullptr)
		return E_FAIL;
	m_vecNpcPart.push_back(pPart);
	
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CNPC_Carlotta::Ready_Component(_uint _iLevel)
{
	__super::Ready_Component();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.f, -1.3f, 12.f, 1.f));
	
	if (g_curLevel != LEVEL_TOOL)
	{
		m_pNavigationCom->Set_CurCell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		_vector	vPosition = m_pNavigationCom->Set_OnCell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}

#pragma region Collider

	m_pVecCollider.resize(CARLOTTA_COL_END);

	/* AABB */
	CBounding_AABB::BOUNDING_AABB_DESC AABBDesc = {};
	AABBDesc.vExtents = _float3(0.5f, 0.7f, 0.5f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), (CComponent**)&m_pVecCollider[CARLOTTA_COL_AABB], &AABBDesc)))
		return E_FAIL;

	m_pVecCollider[CARLOTTA_COL_AABB]->Set_OwnerObj(this);

	/* Dialog */
	CBounding_Sphere::BOUNDING_SPHERE_DESC SphereDesc = {};
	SphereDesc.fRadius = 4.f;
	SphereDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Collider_Sphere"),
		TEXT("Com_Collider_Detection"), (CComponent**)&m_pVecCollider[CARLOTTA_COL_DIALOG], &SphereDesc)))
		return E_FAIL;

	m_pVecCollider[CARLOTTA_COL_DIALOG]->Set_OwnerObj(this);

	/* Active */
	SphereDesc.fRadius = 4.f;
	SphereDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Collider_Sphere"),
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
	m_fHp = 100;
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
