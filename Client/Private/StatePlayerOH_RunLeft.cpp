#include "framework.h"
#include "StatePlayerOH_RunLeft.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_RunLeft::CStatePlayerOH_RunLeft()
{
}

HRESULT CStatePlayerOH_RunLeft::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_RunLeft::Update(_float _fTimeDelta)
{
	Key_Input(_fTimeDelta);
}

void CStatePlayerOH_RunLeft::Late_Update()
{
}

void CStatePlayerOH_RunLeft::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyPress('A'))
	{
		if (m_pPlayer->Get_CamMode() == CPlayer::CAM_3ST)
			m_pPlayerTransform->Go_Foward(_fTimeDelta, m_pPlayerNavigation);
		
		else if (m_pPlayer->Get_CamMode() == CPlayer::CAM_1ST)
			m_pPlayerTransform->Go_Left(_fTimeDelta, m_pPlayerNavigation);

		if (pGameInstance->Get_DIKeyPress('W'))
			m_pPlayer->Set_State(CPlayer::ONEHAND_RUN_F);

		if (pGameInstance->Get_DIKeyPress('S'))
		{
			m_pPlayer->Play_Animation(true, "1hm_runbackward");
			m_pPlayer->Set_State(CPlayer::ONEHAND_RUN_B);
		}

		/* ���� */
		if (pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
		{
			m_pPlayerTransform->SetLook(m_pPlayer->Get_PlayerCamLook());
			m_pPlayerTransform->Set_Speed(m_pPlayer->GetWalkSpeed());

			m_pPlayer->Set_State(CPlayer::ONEHAND_LWATTACKL);
			m_pPlayer->Play_Animation(false, "1hm_walkleftattackleft");
		}
	}

	if (pGameInstance->Get_DIKeyUp('A'))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_IDLE);
		m_pPlayer->Play_Animation(true, "1hm_idle");
	}

	Safe_Release(pGameInstance);

	__super::Key_Input(_fTimeDelta);

}

CStatePlayerOH_RunLeft* CStatePlayerOH_RunLeft::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_RunLeft* pInstance = new CStatePlayerOH_RunLeft();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_RunLeft");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_RunLeft::Free()
{
}
