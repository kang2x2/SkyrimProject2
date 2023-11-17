#include "framework.h"
#include "StateFalmerOH_Chase.h"

#include "GameInstance.h"

#include "Falmer_OneHand.h"
#include "Player.h"

CStateFalmerOH_Chase::CStateFalmerOH_Chase()
{
}

HRESULT CStateFalmerOH_Chase::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerOH_Chase::Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CTransform* pTragetTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));

	m_pMonsterTransform->Go_Foward(_fTimeDelta, m_pMonsterNavigation);
	m_pMonsterTransform->LookAt(pTragetTransform->Get_State(CTransform::STATE_POSITION));

	/* 추격 범위를 벗어났을 때 */
	if (!pGameInstance->Collision_ColCheck(m_pVecCollider[CFalmer_OneHand::FALMEROH_COL_MISSDETECTION], dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
	{
		m_pMonster->Set_State(CFalmer_OneHand::FALMEROH_DETECTION);
		m_pMonster->Play_Animation(false, "idledetection");
	}

	/* RunPowerAtk 범위에 들어왔을 때 */
	if (pGameInstance->Collision_ColCheck(m_pVecCollider[CFalmer_OneHand::FALMEROH_COL_ATKROUND], dynamic_cast<CCollider*>(m_pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB")))))
	{
		m_pMonsterTransform->Set_Speed(m_pMonster->Get_FalmerUESpeedDesc().fChargeSpeed);

		m_pMonster->Set_State(CFalmer_OneHand::FALMEROH_CHARGE);
		m_pMonster->Play_Animation(false, "1hm_forwardpowerattack1");
	}

	Safe_Release(pGameInstance);
}

void CStateFalmerOH_Chase::Late_Update()
{

}

CStateFalmerOH_Chase* CStateFalmerOH_Chase::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerOH_Chase* pInstance = new CStateFalmerOH_Chase();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerOH_Chase");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerOH_Chase::Free()
{
}

