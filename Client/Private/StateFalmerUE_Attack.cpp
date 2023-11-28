#include "framework.h"
#include "StateFalmerUE_Attack.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"
#include "Player.h"

CStateFalmerUE_Attack::CStateFalmerUE_Attack()
{
}

HRESULT CStateFalmerUE_Attack::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerUE_Attack::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CTransform* pTragetTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));

	/* 공격 중 서로의 콜라이더가 충돌하였으면. (피격) */
	if (m_pPlayer->Get_CurState() == CPlayer::ONEHAND_BLOCK)
	{
		if (pGameInstance->Collision_Enter(m_pWeaponCollider, m_pPlayerWeaponCollider))
		{
			m_pMonster->Play_Animation(false, "1hmrecoil1");
			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_STAGGERL);
			m_pPlayer->Set_State(CPlayer::ONEHAND_ANTICIPATE);
			m_pPlayer->Play_Animation_All(false, "1hm_blockanticipate");
		}
	}
	else
	{
		if (pGameInstance->Collision_Enter(m_pWeaponCollider, m_pPlayerBodyCollider))
		{
			//if (m_pMonster->Get_CurFrameIndex() >= 12 &&
			//	m_pMonster->Get_CurFrameIndex() <= 20)
			//{
				// 데미지 처리.
			//}
		}
	}

	Safe_Release(pGameInstance);

}

void CStateFalmerUE_Attack::Late_Update()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* 동작이 끝났을 때 */
	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "1hm_attack1"))
	{
		if (pGameInstance->Collision_Stay(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_ATKROUND], m_pPlayerBodyCollider))
		{
			m_pMonsterTransform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_ATK2);
			m_pMonster->Play_Animation(false, "1hm_attack3");
		}
		else
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_CHASE);
			m_pMonster->Play_Animation(true, "mtrunforward");
		}
	}

	Safe_Release(pGameInstance);
}

CStateFalmerUE_Attack* CStateFalmerUE_Attack::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerUE_Attack* pInstance = new CStateFalmerUE_Attack();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerUE_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerUE_Attack::Free()
{
}

