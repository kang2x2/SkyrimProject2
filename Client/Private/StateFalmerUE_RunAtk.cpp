#include "framework.h"
#include "StateFalmerUE_RunAtk.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"

CStateFalmerUE_RunAtk::CStateFalmerUE_RunAtk()
{
}

HRESULT CStateFalmerUE_RunAtk::Initialize(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerUE_RunAtk::Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>
		(pGameInstance->Find_CloneObject(LEVEL_WHITERUN, TEXT("Layer_Player"), TEXT("Player")));

	CTransform* pTragetTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));

	m_pMonsterTransform->Go_Foward(_fTimeDelta, m_pMonsterNavigation);

	// m_pMonsterTransform->LookAt((dynamic_cast<CMonster*>(m_pMonster)->Get_OriginPos()));

	/* 공격 중 서로의 aabb 박스가 충돌하였으면. (피격) */
	if (pGameInstance->Collision_DetectionPlayer(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_AABB], dynamic_cast<CCollider*>(pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB"))), _fTimeDelta))
	{
		// 데미지 처리.
	}

	Safe_Release(pGameInstance);

}

void CStateFalmerUE_RunAtk::Late_Update()
{
	if (dynamic_cast<CMonster*>(m_pMonster)->Get_IsAnimationFin() &&
		dynamic_cast<CMonster*>(m_pMonster)->Get_CurAnimationName("1hm_forwardpowerattack1"))
	{
		m_pMonsterTransform->Set_Speed(3.f);

		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Set_State(CFalmer_UnEquip::FALMERUE_CHASE);
		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Play_Animation(true, "mtrunforward");
	}
}

CStateFalmerUE_RunAtk* CStateFalmerUE_RunAtk::Create(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerUE_RunAtk* pInstance = new CStateFalmerUE_RunAtk();

	if (FAILED(pInstance->Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerUE_RunAtk");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerUE_RunAtk::Free()
{
}

