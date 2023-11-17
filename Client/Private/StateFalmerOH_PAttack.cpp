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
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CTransform* pTragetTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));

	/* ���� �� ������ �ݶ��̴��� �浹�Ͽ�����. (�ǰ�) */
	if (m_pPlayer->Get_CurState() == CPlayer::ONEHAND_BLOCK)
	{
		if (m_isReadyAtk && pGameInstance->Collision_ColCheck(dynamic_cast<CCollider*>(m_pMonster->Get_Part(CFalmer_OneHand::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB"))), dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB")))))
		{
			m_isReadyAtk = false;
			m_pMonsterTransform->Set_Speed(m_pMonster->GetWalkSpeed());
			m_pMonster->Play_Animation(false, "1hmrecoillarge");
			m_pMonster->Set_State(CFalmer_OneHand::FALMEROH_STAGGERH);
			m_pPlayer->Set_State(CPlayer::ONEHAND_ANTICIPATE);
			m_pPlayer->Play_Animation(false, "1hm_blockanticipate");
		}
	}

	else if (m_isReadyAtk && pGameInstance->Collision_ColCheck(dynamic_cast<CCollider*>(m_pMonster->Get_Part(CFalmer_OneHand::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB"))), dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
	{
		//if (m_pMonster->Get_CurFrameIndex() >= 12 &&
		//	m_pMonster->Get_CurFrameIndex() <= 20)
		//{
			// ������ ó��.
		m_isReadyAtk = false;
		//}
	}

	Safe_Release(pGameInstance);

}

void CStateFalmerOH_PAttack::Late_Update()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* ������ ������ �� */
	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "1hm_standingpowerattack1"))
	{
		m_isReadyAtk = true;
		if (!pGameInstance->Collision_ColCheck(m_pVecCollider[CFalmer_OneHand::FALMEROH_COL_ATKROUND], dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CFalmer_OneHand::FALMEROH_CHASE);
			m_pMonster->Play_Animation(true, "mtrunforward");
		}
		else
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CFalmer_OneHand::FALMEROH_ATK);
			m_pMonster->Play_Animation(false, "1hm_attack2");
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
