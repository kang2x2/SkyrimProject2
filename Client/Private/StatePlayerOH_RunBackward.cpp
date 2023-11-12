#include "framework.h"
#include "StatePlayerOH_RunBackward.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_RunBackward::CStatePlayerOH_RunBackward()
{
}

HRESULT CStatePlayerOH_RunBackward::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_RunBackward::Update(_float _fTimeDelta)
{
	Key_Input(_fTimeDelta);
}

void CStatePlayerOH_RunBackward::Late_Update()
{
}

void CStatePlayerOH_RunBackward::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyPress('S'))
	{
		m_pPlayerTransform->SetLook(m_pPlayer->Get_PlayerCamLook());

		if (pGameInstance->Get_DIKeyPress('A'))
		{
			if (m_pPlayer->Get_CamMode() == CPlayer::CAM_3ST)
			{
				Player_SetLook(45.f);
				m_pPlayerTransform->Go_Backward(_fTimeDelta, m_pPlayerNavigation);
			}

			else if (m_pPlayer->Get_CamMode() == CPlayer::CAM_1ST)
			{
				m_pPlayerTransform->Go_Backward(_fTimeDelta, m_pPlayerNavigation);
				m_pPlayerTransform->Go_Left(_fTimeDelta, m_pPlayerNavigation);
			}

			if (pGameInstance->Get_DIKeyUp('S'))
			{
				Player_SetLook(135.f);

				m_pPlayer->Play_Animation(true, "1hm_runforward");
				m_pPlayer->Set_State(CPlayer::ONEHAND_RUN_L);
			}

			/* ���� */
			if (pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
			{
				m_pPlayerTransform->Set_Speed(m_pPlayer->GetWalkSpeed());

				m_pPlayer->Set_State(CPlayer::ONEHAND_LATTACKB);
				m_pPlayer->Play_Animation(false, "1hm_walkbwdattackleft");
			}
		}

		else if (pGameInstance->Get_DIKeyPress('D'))
		{
			if (m_pPlayer->Get_CamMode() == CPlayer::CAM_3ST)
			{
				Player_SetLook(-45.f);
				m_pPlayerTransform->Go_Backward(_fTimeDelta, m_pPlayerNavigation);
			}

			else if (m_pPlayer->Get_CamMode() == CPlayer::CAM_1ST)
			{
				m_pPlayerTransform->Go_Backward(_fTimeDelta, m_pPlayerNavigation);
				m_pPlayerTransform->Go_Right(_fTimeDelta, m_pPlayerNavigation);
			}

			if (pGameInstance->Get_DIKeyUp('S'))
			{
				Player_SetLook(135.f);

				m_pPlayer->Play_Animation(true, "1hm_runforward");
				m_pPlayer->Set_State(CPlayer::ONEHAND_RUN_R);
			}

			/* ���� */
			if (pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
			{
				m_pPlayerTransform->Set_Speed(m_pPlayer->GetWalkSpeed());

				m_pPlayer->Set_State(CPlayer::ONEHAND_LATTACKB);
				m_pPlayer->Play_Animation(false, "1hm_walkbwdattackleft");
			}
		}

		/* ���� */
		else if (pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
		{
			m_pPlayerTransform->Set_Speed(m_pPlayer->GetWalkSpeed());

			m_pPlayer->Set_State(CPlayer::ONEHAND_LATTACKB);
			m_pPlayer->Play_Animation(false, "1hm_walkbwdattackleft");
		}

		else
			m_pPlayerTransform->Go_Backward(_fTimeDelta, m_pPlayerNavigation);
	}

	if (pGameInstance->Get_DIKeyUp('S'))
	{
		m_pPlayer->Play_Animation(true, "1hm_idle");
		m_pPlayer->Set_State(CPlayer::ONEHAND_IDLE);
	}

	Safe_Release(pGameInstance);

	__super::Key_Input();
}

CStatePlayerOH_RunBackward* CStatePlayerOH_RunBackward::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_RunBackward* pInstance = new CStatePlayerOH_RunBackward();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_RunBackward");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_RunBackward::Free()
{
}
