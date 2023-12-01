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
	__super::Update(_fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* ���� �� ������ �ݶ��̴��� �浹�Ͽ�����. (�ǰ�) */
	if (m_pPlayer->Get_CurState() == CPlayer::ONEHAND_BLOCK ||
		m_pPlayer->Get_CurState() == CPlayer::ONEHAND_ANTICIPATE)
	{
		if (pGameInstance->Collision_Enter(m_pWeaponCollider, m_pPlayerWeaponCollider))
		{
			m_pMonster->Play_Animation(false, "1hmrecoil3");
			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_STAGGERL2);
			m_pPlayer->Set_State(CPlayer::ONEHAND_ANTICIPATE);
			m_pPlayer->Play_Animation_All(false, "1hm_blockanticipate");
		}
	}
	else
	{
		if (!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "1hm_attack3") &&
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

void CStateFalmerUE_Attack2::Late_Update()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* ������ ������ �� */
	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "1hm_attack3"))
	{
		if (pGameInstance->Collision_Stay(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_ATKROUND], m_pPlayerBodyCollider))
		{
			m_pMonsterTransform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_ATK);
			m_pMonster->Play_Animation(false, "1hm_attack1");
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
