#include "framework.h"
#include "StateFalmerUE_Chase.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"

CStateFalmerUE_Chase::CStateFalmerUE_Chase()
{
}

HRESULT CStateFalmerUE_Chase::Initialize(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerUE_Chase::Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>
		(pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Player")));

	CTransform* pTragetTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));

	m_pMonsterTransform->Go_Foward(_fTimeDelta, m_pMonsterNavigation);
	m_pMonsterTransform->LookAt(pTragetTransform->Get_State(CTransform::STATE_POSITION));

	/* 추격 범위를 벗어났을 때 */
	if (!pGameInstance->Collision_ColCheck(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_MISSDETECTION], dynamic_cast<CCollider*>(pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
	{
		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Set_State(CFalmer_UnEquip::FALMERUE_DETECTION);
		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Play_Animation(false, "idledetection");
	}

	/* RunPowerAtk 범위에 들어왔을 때 */
	if (pGameInstance->Collision_ColCheck(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_ATKROUND], dynamic_cast<CCollider*>(pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
	{
		m_pMonsterTransform->Set_Speed(dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Get_FalmerUESpeedDesc().fChargeSpeed);

		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Set_State(CFalmer_UnEquip::FALMERUE_CHARGE);
		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Play_Animation(false, "1hm_forwardpowerattack1");
	}

	Safe_Release(pGameInstance);
}

void CStateFalmerUE_Chase::Late_Update()
{

}

CStateFalmerUE_Chase* CStateFalmerUE_Chase::Create(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerUE_Chase* pInstance = new CStateFalmerUE_Chase();

	if (FAILED(pInstance->Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerUE_Chase");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerUE_Chase::Free()
{
}

