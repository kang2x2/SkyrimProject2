#include "framework.h"
#include "StateFalmerOH_Charge.h"

#include "GameInstance.h"

#include "Falmer_OneHand.h"
#include "Player.h"

CStateFalmerOH_Charge::CStateFalmerOH_Charge()
{
}

HRESULT CStateFalmerOH_Charge::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerOH_Charge::Update(_float _fTimeDelta)
{

	m_pMonsterTransform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

	if (m_pMonster->Get_CurFrameIndex() < 30)
		m_pMonsterTransform->Go_Foward(_fTimeDelta, m_pMonsterNavigation);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	/* 공격 중 서로의 aabb 박스가 충돌하였으면. (피격) */
	if (m_pPlayer->Get_CurState() == CPlayer::ONEHAND_BLOCK)
	{
		if (pGameInstance->Collision_Enter(m_pWeaponCollider, m_pPlayerWeaponCollider))
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetWalkSpeed());
			m_pMonster->Play_Animation(false, "1hmstaggerbackmid");
			m_pMonster->Set_State(CFalmer_OneHand::FALMEROH_STAGGERH2);
			m_pPlayer->Set_State(CPlayer::ONEHAND_BLOCKHIT);
			m_pPlayer->Play_Animation(false, "1hm_blockhit");
			m_pPlayerTransform->Set_Speed(m_pPlayer->GetWalkSpeed());
		}
	}
	else
	{
		if (pGameInstance->Collision_Enter(m_pWeaponCollider, m_pPlayerBodyCollider))
		{
			// 데미지 처리.
		}
	}

	Safe_Release(pGameInstance);

}

void CStateFalmerOH_Charge::Late_Update()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* 동작이 끝났을 때 */
	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "1hm_forwardpowerattack1"))
	{
		if (pGameInstance->Collision_Stay(m_pVecCollider[CFalmer_OneHand::FALMEROH_COL_ATKROUND], m_pPlayerBodyCollider))
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

CStateFalmerOH_Charge* CStateFalmerOH_Charge::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerOH_Charge* pInstance = new CStateFalmerOH_Charge();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerOH_Charge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerOH_Charge::Free()
{
}

