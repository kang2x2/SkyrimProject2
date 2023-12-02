#include "framework.h"
#include "StateFalmerOH_PAttack.h"

#include "GameInstance.h"

#include "Falmer_OneHand.h"
#include "Player.h"

CStateFalmerOH_PAttack::CStateFalmerOH_PAttack()
{
}

HRESULT CStateFalmerOH_PAttack::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerOH_PAttack::Update(_float _fTimeDelta)
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
			{
				m_pPlayer->Set_IsCounter(true);
				m_pPlayer->Set_State(CPlayer::ONEHAND_ANTICIPATE);
				m_pPlayer->Play_Animation_All(false, "1hm_blockanticipate");
			}
			else
			{
				m_pPlayer->Set_State(CPlayer::ONEHAND_BLOCKHIT);
				m_pPlayer->Play_Animation_All(false, "1hm_blockhit");
				m_pPlayerTransform->Set_Speed(m_pPlayer->GetWalkSpeed());
			}

			m_pMonsterTransform->Set_Speed(m_pMonster->GetWalkSpeed());
			m_pMonster->Play_Animation(false, "1hmrecoillarge");
			m_pMonster->Set_State(CFalmer_OneHand::FALMEROH_STAGGERH);

		}
	}

	else 
	{
		if (!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "1hm_standingpowerattack1") &&
			m_pMonster->Get_CurFrameIndex() >= 20 && m_pMonster->Get_CurFrameIndex() <= 30)
		{
			if (pGameInstance->Collision_Enter(m_pWeaponCollider, m_pPlayerBodyCollider))
			{
				m_pPlayer->SetHp(-20.f);
				m_pPlayer->Set_IsHit(true);
			}
		}

	}

	Safe_Release(pGameInstance);

}

void CStateFalmerOH_PAttack::Late_Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* ������ ������ �� */
	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "1hm_standingpowerattack1"))
	{
		if (pGameInstance->Collision_Stay(m_pVecCollider[CFalmer_OneHand::FALMEROH_COL_ATKROUND], dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
		{
			m_pMonsterTransform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CFalmer_OneHand::FALMEROH_ATK);
			m_pMonster->Play_Animation(false, "1hm_attack2");
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

CStateFalmerOH_PAttack* CStateFalmerOH_PAttack::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerOH_PAttack* pInstance = new CStateFalmerOH_PAttack();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerOH_PAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerOH_PAttack::Free()
{
}
