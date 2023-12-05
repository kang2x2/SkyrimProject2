#include "framework.h"
#include "StateGuardCaptain_Idle.h"

#include "GameInstance.h"

#include "NPC_GuardCaptain.h"
#include "Player.h"

CStateGuardCaptain_Idle::CStateGuardCaptain_Idle()
{
}

HRESULT CStateGuardCaptain_Idle::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateGuardCaptain_Idle::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Collision_Enter(m_pVecCollider[CNPC_GuardCaptain::GUARDCAPTAIN_COL_DETECTION], m_pPlayerBodyCollider))
	{
		if (m_pNpc->Get_CurTalkID() == 0)
		{
			m_pNpcTransform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

			m_pNpc->Play_Animation(false, "1hm_equip");
			m_pNpc->Set_State(CNPC_GuardCaptain::GUARDCAPTAIN_EQUIP);
		}
	}

	Safe_Release(pGameInstance);
}

void CStateGuardCaptain_Idle::Late_Update(_float _fTimeDelta)
{

}

CStateGuardCaptain_Idle* CStateGuardCaptain_Idle::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateGuardCaptain_Idle* pInstance = new CStateGuardCaptain_Idle();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateGuardCaptain_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateGuardCaptain_Idle::Free()
{
}

