#include "framework.h"
#include "StateFalmerUE_Chase.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"
#include "Player.h"

CStateFalmerUE_Chase::CStateFalmerUE_Chase()
{
}

HRESULT CStateFalmerUE_Chase::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerUE_Chase::Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pMonsterTransform->Go_Foward(_fTimeDelta, m_pMonsterNavigation);
	m_pMonsterTransform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

	/* �߰� ������ ����� �� */
	if (!pGameInstance->Collision_Stay(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_MISSDETECTION], m_pPlayerBodyCollider))
	{
		m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_DETECTION);
		m_pMonster->Play_Animation(false, "idledetection");
	}

	/* RunPowerAtk ������ ������ �� */
	if (pGameInstance->Collision_Enter(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_ATKROUND], m_pPlayerBodyCollider))
	{
		m_pMonsterTransform->Set_Speed(m_pMonster->Get_FalmerUESpeedDesc().fChargeSpeed);

		m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_CHARGE);
		m_pMonster->Play_Animation(false, "1hm_forwardpowerattack1");
	}

	Safe_Release(pGameInstance);
}

void CStateFalmerUE_Chase::Late_Update()
{

}

CStateFalmerUE_Chase* CStateFalmerUE_Chase::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerUE_Chase* pInstance = new CStateFalmerUE_Chase();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerUE_Chase");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerUE_Chase::Free()
{
}

