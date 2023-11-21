#include "framework.h"
#include "StateSkeever_Charge.h"

#include "GameInstance.h"

#include "Skeever.h"
#include "Player.h"

CStateSkeever_Charge::CStateSkeever_Charge()
{
}

HRESULT CStateSkeever_Charge::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateSkeever_Charge::Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_pMonster->Get_CurFrameIndex() < 30)
		m_pMonsterTransform->Go_Foward(_fTimeDelta, m_pMonsterNavigation);

	// m_pMonsterTransform->LookAt((dynamic_cast<CMonster*>(m_pMonster)->Get_OriginPos()));

	/* 공격 중 서로의 aabb 박스가 충돌하였으면. (피격) */
	if (m_pPlayer->Get_CurState() == CPlayer::ONEHAND_BLOCK)
	{
		if (m_isReadyAtk && pGameInstance->Collision_Enter(dynamic_cast<CCollider*>(m_pMonster->Get_Part(CSkeever::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB"))), dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB")))))
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetWalkSpeed());
			m_isReadyAtk = false;
			m_pMonster->Play_Animation(false, "recoil");
			m_pMonster->Set_State(CSkeever::SKEEVER_STAGGER);
			m_pPlayer->Set_State(CPlayer::ONEHAND_BLOCKHIT);
			m_pPlayer->Play_Animation_All(false, "1hm_blockhit");
			dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")))->Set_Speed(m_pPlayer->GetWalkSpeed());
		}
	}
	if (m_isReadyAtk && pGameInstance->Collision_Enter(dynamic_cast<CCollider*>(m_pMonster->Get_Part(CSkeever::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB"))), dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
	{
		//if (m_pMonster->Get_CurFrameIndex() >= 27 &&
		//	m_pMonster->Get_CurFrameIndex() <= 30)
		//{
		m_isReadyAtk = false;
		// 데미지 처리.
		int i = 0;
		//}
	}

	Safe_Release(pGameInstance);

}

void CStateSkeever_Charge::Late_Update()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CTransform* pTragetTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
	m_pMonsterTransform->LookAt(pTragetTransform->Get_State(CTransform::STATE_POSITION));

	/* 동작이 끝났을 때 */
	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "attackpowerforward"))
	{
		m_isReadyAtk = true;
		/* 추격 범위를 벗어났을 때 */
		if (!pGameInstance->Collision_Enter(m_pVecCollider[CSkeever::SKEEVER_COL_MISSDETECTION], dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
		{
			m_pMonster->Set_State(CSkeever::SKEEVER_DETECTION);
			m_pMonster->Play_Animation(false, "idlecombat1");
		}

		else if (!pGameInstance->Collision_Enter(m_pVecCollider[CSkeever::SKEEVER_COL_ATKROUND], dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CSkeever::SKEEVER_CHASE);
			m_pMonster->Play_Animation(true, "runforward");
		}

		else
		{
			m_pMonsterTransform->LookAt(pTragetTransform->Get_State(CTransform::STATE_POSITION));

			m_pMonster->Set_State(CSkeever::SKEEVER_ATK);
			m_pMonster->Play_Animation(false, "attack2");
		}
	}

	Safe_Release(pGameInstance);
}

CStateSkeever_Charge* CStateSkeever_Charge::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateSkeever_Charge* pInstance = new CStateSkeever_Charge();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateSkeever_Charge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateSkeever_Charge::Free()
{
}
