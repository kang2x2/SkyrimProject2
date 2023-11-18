#include "framework.h"
#include "StateSkeever_Return.h"

#include "GameInstance.h"

#include "Skeever.h"
#include "Player.h"

CStateSkeever_Return::CStateSkeever_Return()
{
}

HRESULT CStateSkeever_Return::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateSkeever_Return::Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CTransform* pTragetTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));

	m_pMonsterTransform->Go_Foward(_fTimeDelta, m_pMonsterNavigation);

	// m_pMonsterTransform->Chase(dynamic_cast<CMonster*>(m_pMonster)->Get_OriginPos(), _fTimeDelta, 1.5f);
	m_pMonsterTransform->LookAt(m_pMonster->Get_OriginPos());

	if (pGameInstance->Collision_Enter(m_pVecCollider[CSkeever::SKEEVER_COL_DETECTION], dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
	{
		m_pMonster->Set_State(CSkeever::SKEEVER_WARNING);
		m_pMonster->Play_Animation(false, "aggrowarning1");
	}

	Safe_Release(pGameInstance);

}

void CStateSkeever_Return::Late_Update()
{
	_float4 vPos = {};
	_float4 vOriginPos = {};

	XMStoreFloat4(&vPos, m_pMonsterTransform->Get_State(CTransform::STATE_POSITION));
	XMStoreFloat4(&vOriginPos, m_pMonster->Get_OriginPos());

	if (vPos.x >= vOriginPos.x - 1.f && vPos.x <= vOriginPos.x + 1.f &&
		vPos.z >= vOriginPos.z - 2.f && vPos.z <= vOriginPos.z + 2.f)
	{
		m_pMonster->Set_State(CSkeever::SKEEVER_IDLE);
		m_pMonster->Play_Animation(true, "combatidle");
	}

}

CStateSkeever_Return* CStateSkeever_Return::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateSkeever_Return* pInstance = new CStateSkeever_Return();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateSkeever_Return");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateSkeever_Return::Free()
{
}
