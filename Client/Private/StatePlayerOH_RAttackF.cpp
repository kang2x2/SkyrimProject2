#include "framework.h"
#include "StatePlayerOH_RAttackF.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_RAttackF::CStatePlayerOH_RAttackF()
{
}

HRESULT CStatePlayerOH_RAttackF::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_RAttackF::Update(_float _fTimeDelta)
{
	Key_Input(_fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_pPlayer->Get_CurFrameIndex() >= 25 &&
		m_pPlayer->Get_CurAnimationName("1hm_walkfwdattackright") &&
		pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_LATTACKF);
		m_pPlayer->Play_Animation(false, "1hm_walkfwdattackleft");
	}

	Safe_Release(pGameInstance);
}

void CStatePlayerOH_RAttackF::Late_Update()
{
	__super::Key_Input();

	if (m_pPlayer->Get_IsAnimationFin())
	{
		m_pPlayerTransform->Set_Speed(m_pPlayer->GetRunSpeed());

		m_pPlayer->Set_State(CPlayer::ONEHAND_IDLE);
		m_pPlayer->Play_Animation(true, "1hm_idle");
	}
}

void CStatePlayerOH_RAttackF::Key_Input(_float _fTimeDelta)
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

CStatePlayerOH_RAttackF* CStatePlayerOH_RAttackF::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_RAttackF* pInstance = new CStatePlayerOH_RAttackF();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_RAttackF");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_RAttackF::Free()
{
}
