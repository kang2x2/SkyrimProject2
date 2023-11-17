#include "framework.h"
#include "StateFalmerUE_Attack2.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"
#include "Player.h"

CStateFalmerUE_Attack2::CStateFalmerUE_Attack2()
{
}

HRESULT CStateFalmerUE_Attack2::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerUE_Attack2::Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CTransform* pTragetTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));

	/* 공격 중 서로의 콜라이더가 충돌하였으면. (피격) */
	if (m_pPlayer->Get_CurState() == CPlayer::ONEHAND_BLOCK)
	{
		if (m_isReadyAtk && pGameInstance->Collision_ColCheck(dynamic_cast<CCollider*>(m_pMonster->Get_Part(CFalmer_UnEquip::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB"))), dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB")))))
		{
			m_isReadyAtk = false;
			m_pMonster->Play_Animation(false, "1hmrecoil3");
			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_STAGGERL2);
			m_pPlayer->Set_State(CPlayer::ONEHAND_ANTICIPATE);
			m_pPlayer->Play_Animation(false, "1hm_blockanticipate");
		}
	}

	else if (m_isReadyAtk && pGameInstance->Collision_ColCheck(dynamic_cast<CCollider*>(m_pMonster->Get_Part(CFalmer_UnEquip::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB"))), dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
	{
		//if (m_pMonster->Get_CurFrameIndex() >= 12 &&
		//	m_pMonster->Get_CurFrameIndex() <= 20)
		//{
			// 데미지 처리.
		m_isReadyAtk = false;
		//}
	}

	Safe_Release(pGameInstance);

}

void CStateFalmerUE_Attack2::Late_Update()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* 동작이 끝났을 때 */
	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "1hm_attack3"))
	{
		m_isReadyAtk = true;
		if (!pGameInstance->Collision_ColCheck(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_ATKROUND], dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_CHASE);
			m_pMonster->Play_Animation(true, "mtrunforward");
		}
		else
		{
			CTransform* pTragetTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
			m_pMonsterTransform->LookAt(pTragetTransform->Get_State(CTransform::STATE_POSITION));
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_ATK);
			m_pMonster->Play_Animation(false, "1hm_attack1");
		}
	}

	Safe_Release(pGameInstance);

}

CStateFalmerUE_Attack2* CStateFalmerUE_Attack2::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerUE_Attack2* pInstance = new CStateFalmerUE_Attack2();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerUE_Attack2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerUE_Attack2::Free()
{
}
