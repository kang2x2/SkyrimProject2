#include "framework.h"
#include "StateFalmerOH_Attack.h"

#include "GameInstance.h"

#include "Falmer_OneHand.h"
#include "Player.h"

CStateFalmerOH_Attack::CStateFalmerOH_Attack()
{
}

HRESULT CStateFalmerOH_Attack::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerOH_Attack::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* ���� �� ������ �ݶ��̴��� �浹�Ͽ�����. (�ǰ�) */
	if (!strcmp(m_pPlayer->Get_CurAnimationName().c_str(), "1hm_blockidle") ||
		m_pPlayer->Get_CurState() == CPlayer::ONEHAND_ANTICIPATE)
	{
		if (pGameInstance->Collision_Enter(m_pWeaponCollider, m_pPlayerWeaponCollider))
		{
			if (m_pPlayer->Get_IsReadyCounter())
				m_pPlayer->Set_IsCounter(true);

			m_pMonster->Play_Animation(false, "1hmrecoil1");
			m_pMonster->Set_State(CFalmer_OneHand::FALMEROH_STAGGERL);
			m_pPlayer->Set_State(CPlayer::ONEHAND_ANTICIPATE);
			m_pPlayer->Play_Animation_All(false, "1hm_blockanticipate");
		}
	}

	else
	{
		if (!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "1hm_attack2") &&
			m_pMonster->Get_CurFrameIndex() >= 12 && m_pMonster->Get_CurFrameIndex() <= 20)
		{
			if (pGameInstance->Collision_Enter(m_pWeaponCollider, m_pPlayerBodyCollider))
			{
				m_pPlayer->SetHp(-10.f);
				m_pPlayer->Set_IsHit(true);
			}
		}
	}

	Safe_Release(pGameInstance);

}

void CStateFalmerOH_Attack::Late_Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* ������ ������ �� */
	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "1hm_attack2"))
	{
		if (pGameInstance->Collision_Stay(m_pVecCollider[CFalmer_OneHand::FALMEROH_COL_ATKROUND], m_pPlayerBodyCollider))
		{
			m_pMonsterTransform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CFalmer_OneHand::FALMEROH_ATK2);
			m_pMonster->Play_Animation(false, "1hm_attack3");
		}
		else
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CFalmer_OneHand::FALMEROH_CHASE);
			m_pMonster->Play_Animation(true, "mtrunforward");
		}
	}

	Safe_Release(pGameInstance);
}

CStateFalmerOH_Attack* CStateFalmerOH_Attack::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerOH_Attack* pInstance = new CStateFalmerOH_Attack();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerOH_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerOH_Attack::Free()
{
}

