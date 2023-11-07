#include "framework.h"
#include "StateFalmerUE_Charge.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"

CStateFalmerUE_Charge::CStateFalmerUE_Charge()
{
}

HRESULT CStateFalmerUE_Charge::Initialize(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerUE_Charge::Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>
		(pGameInstance->Find_CloneObject(LEVEL_WHITERUN, TEXT("Layer_Player"), TEXT("Player")));

	CTransform* pTragetTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));

	if(dynamic_cast<CMonster*>(m_pMonster)->Get_CurFrameIndex() < 30)
		m_pMonsterTransform->Go_Foward(_fTimeDelta, m_pMonsterNavigation);

	/* 동작이 끝났을 때 */
	if (dynamic_cast<CMonster*>(m_pMonster)->Get_IsAnimationFin() &&
		dynamic_cast<CMonster*>(m_pMonster)->Get_CurAnimationName("1hm_forwardpowerattack1"))
	{
		/* 추격 범위를 벗어났을 때 */
		if (!pGameInstance->Collision_ColCheck(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_MISSDETECTION], dynamic_cast<CCollider*>(pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
		{
			dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Set_State(CFalmer_UnEquip::FALMERUE_DETECTION);
			dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Play_Animation(false, "idledetection");
		}

		else if (!pGameInstance->Collision_ColCheck(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_ATKROUND], dynamic_cast<CCollider*>(pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
		{
			m_pMonsterTransform->Set_Speed(dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->GetRunSpeed());

			dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Set_State(CFalmer_UnEquip::FALMERUE_CHASE);
			dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Play_Animation(true, "mtrunforward");
		}

		else
		{
			m_pMonsterTransform->LookAt(pTragetTransform->Get_State(CTransform::STATE_POSITION));

			dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Set_State(CFalmer_UnEquip::FALMERUE_ATK);
			dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Play_Animation(false, "1hm_attack1");
		}
	}

	// m_pMonsterTransform->LookAt((dynamic_cast<CMonster*>(m_pMonster)->Get_OriginPos()));

	/* 공격 중 서로의 aabb 박스가 충돌하였으면. (피격) */
	if (pGameInstance->Collision_ColCheck(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_ATKAABB], dynamic_cast<CCollider*>(pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
	{
		if (dynamic_cast<CMonster*>(m_pMonster)->Get_CurFrameIndex() >= 27 &&
			dynamic_cast<CMonster*>(m_pMonster)->Get_CurFrameIndex() <= 30)
		{
			// 데미지 처리.
			int i = 0;
		}
	}

	Safe_Release(pGameInstance);

}

void CStateFalmerUE_Charge::Late_Update()
{

}

CStateFalmerUE_Charge* CStateFalmerUE_Charge::Create(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerUE_Charge* pInstance = new CStateFalmerUE_Charge();

	if (FAILED(pInstance->Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerUE_Charge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerUE_Charge::Free()
{
}

