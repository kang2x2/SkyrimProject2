#include "framework.h"
#include "StatePlayerOH_LAttackF.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_LAttackF::CStatePlayerOH_LAttackF()
{
}

HRESULT CStatePlayerOH_LAttackF::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_LAttackF::Update(_float _fTimeDelta)
{
	Key_Input(_fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_pPlayer->Get_CurFrameIndex() >= 25 &&
		m_pPlayer->Get_CurAnimationName("1hm_walkfwdattackleft") &&
		pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_RATTACKF);
		m_pPlayer->Play_Animation(false, "1hm_walkfwdattackright");
	}

	Safe_Release(pGameInstance);

	__super::Key_Input();
}

void CStatePlayerOH_LAttackF::Late_Update()
{
	if (m_pPlayer->Get_IsAnimationFin())
	{
		m_pPlayerTransform->Set_Speed(m_pPlayer->GetRunSpeed());

		m_pPlayer->Set_State(CPlayer::ONEHAND_IDLE);
		m_pPlayer->Play_Animation(true, "1hm_idle");
	}
}

void CStatePlayerOH_LAttackF::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyPress('W'))
	{
		m_pPlayerTransform->Go_Foward(_fTimeDelta, m_pPlayerNavigation);
	}

	else if (pGameInstance->Get_DIKeyPress('S'))
	{
		m_pPlayerTransform->Go_Backward(_fTimeDelta, m_pPlayerNavigation);
	}

	Safe_Release(pGameInstance);

}

CStatePlayerOH_LAttackF* CStatePlayerOH_LAttackF::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_LAttackF* pInstance = new CStatePlayerOH_LAttackF();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_LAttackF");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_LAttackF::Free()
{
}
