#include "framework.h"
#include "StateFalmerUE_Charge.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"
#include "Player.h"

CStateFalmerUE_Charge::CStateFalmerUE_Charge()
{
}

HRESULT CStateFalmerUE_Charge::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerUE_Charge::Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CTransform* pTragetTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
	m_pMonsterTransform->LookAt(pTragetTransform->Get_State(CTransform::STATE_POSITION));

	if(m_pMonster->Get_CurFrameIndex() < 30)
		m_pMonsterTransform->Go_Foward(_fTimeDelta, m_pMonsterNavigation);

	/* 동작이 끝났을 때 */
	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "1hm_forwardpowerattack1"))
	{
		m_isReadyAtk = true;
		/* 추격 범위를 벗어났을 때 */
		if (!pGameInstance->Collision_ColCheck(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_MISSDETECTION], dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
		{
			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_DETECTION);
			m_pMonster->Play_Animation(false, "idledetection");
		}

		else if (!pGameInstance->Collision_ColCheck(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_ATKROUND], dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_CHASE);
			m_pMonster->Play_Animation(true, "mtrunforward");
		}

		else
		{
			m_pMonsterTransform->LookAt(pTragetTransform->Get_State(CTransform::STATE_POSITION));

			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_ATK);
			m_pMonster->Play_Animation(false, "1hm_attack1");
		}
	}

	// m_pMonsterTransform->LookAt((dynamic_cast<CMonster*>(m_pMonster)->Get_OriginPos()));

	/* 공격 중 서로의 aabb 박스가 충돌하였으면. (피격) */
	if (m_pPlayer->Get_CurState() == CPlayer::ONEHAND_BLOCK)
	{
		if (m_isReadyAtk && pGameInstance->Collision_ColCheck(dynamic_cast<CCollider*>(m_pMonster->Get_Part(CFalmer_UnEquip::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB"))), dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB")))))
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetWalkSpeed());
			m_isReadyAtk = false;
			m_pMonster->Play_Animation(false, "1hmstaggerbackheavy");
			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_STAGGERH);
			m_pPlayer->Set_State(CPlayer::ONEHAND_BLOCKHIT);
			m_pPlayer->Play_Animation(false, "1hm_blockhit");
			dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")))->Set_Speed(m_pPlayer->GetWalkSpeed());
		}
	}
	if (m_isReadyAtk && pGameInstance->Collision_ColCheck(dynamic_cast<CCollider*>(m_pMonster->Get_Part(CFalmer_UnEquip::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB"))), dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
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

void CStateFalmerUE_Charge::Late_Update()
{

}

CStateFalmerUE_Charge* CStateFalmerUE_Charge::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerUE_Charge* pInstance = new CStateFalmerUE_Charge();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerUE_Charge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerUE_Charge::Free()
{
}

