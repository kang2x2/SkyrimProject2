#include "framework.h"
#include "StateSkeever_Attack.h"

#include "GameInstance.h"

#include "Skeever.h"
#include "Player.h"

CStateSkeever_Attack::CStateSkeever_Attack()
{
}

HRESULT CStateSkeever_Attack::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateSkeever_Attack::Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CTransform* pTragetTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));

	/* 공격 중 서로의 콜라이더가 충돌하였으면. (피격) */
	if (m_pPlayer->Get_CurState() == CPlayer::ONEHAND_BLOCK)
	{
		if (m_isReadyAtk && pGameInstance->Collision_Enter(dynamic_cast<CCollider*>(m_pMonster->Get_Part(CSkeever::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB"))), dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB")))))
		{
			m_isReadyAtk = false;
			m_pMonster->Play_Animation(false, "recoil");
			m_pMonster->Set_State(CSkeever::SKEEVER_STAGGER);
			m_pPlayer->Set_State(CPlayer::ONEHAND_ANTICIPATE);
			m_pPlayer->Play_Animation(false, "1hm_blockanticipate");
		}
	}


	if (m_isReadyAtk && pGameInstance->Collision_Enter(dynamic_cast<CCollider*>(m_pMonster->Get_Part(CSkeever::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB"))), dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
	{
		//if (m_pMonster->Get_CurFrameIndex() >= 12 &&
		//	m_pMonster->Get_CurFrameIndex() <= 20)
		//{
			// 데미지 처리.
		m_isReadyAtk = false;
		//}
	}

	/* 동작이 끝났을 때 */
	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "attack2"))
	{
		m_isReadyAtk = true;
		if (!pGameInstance->Collision_Enter(m_pVecCollider[CSkeever::SKEEVER_COL_ATKROUND], dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CSkeever::SKEEVER_CHASE);
			m_pMonster->Play_Animation(true, "runforward");
		}
		else
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());
		
			m_pMonster->Set_State(CSkeever::SKEEVER_ATK);
			m_pMonster->Play_Animation(true, "attack2");
		}
	}

	// m_pMonsterTransform->LookAt((dynamic_cast<CMonster*>(m_pMonster)->Get_OriginPos()));

	Safe_Release(pGameInstance);


}

void CStateSkeever_Attack::Late_Update()
{
}

CStateSkeever_Attack* CStateSkeever_Attack::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateSkeever_Attack* pInstance = new CStateSkeever_Attack();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateSkeever_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateSkeever_Attack::Free()
{
}
