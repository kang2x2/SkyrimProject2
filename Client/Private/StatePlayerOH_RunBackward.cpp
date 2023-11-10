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
	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_1ST)
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

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
		m_pPlayerTransform->Go_Backward(_fTimeDelta, m_pPlayerNavigation);

		if (pGameInstance->Get_DIKeyPress('A'))
		{
			if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_3ST)
			{
				m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

				_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
				_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

				m_pPlayerTransform->SetLook(vPlayerLook);
			}

			else if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_1ST)
			{
				m_pPlayerTransform->Go_Left(_fTimeDelta, m_pPlayerNavigation);
			}

			if (pGameInstance->Get_DIKeyUp('S'))
			{
				_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(135.f));
				_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

				m_pPlayerTransform->SetLook(vPlayerLook);

				dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_runforward");
				dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RUN_L);
			}

			/* 공격 */
			else if (pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
			{
				m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());
				m_pPlayerTransform->Set_Speed(dynamic_cast<CPlayer*>(m_pPlayer)->GetWalkSpeed());

				dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_LATTACKB);
				dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_walkbwdattackleft");
			}
		}

		else if (pGameInstance->Get_DIKeyPress('D'))
		{
			if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_1ST)
			{
				m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

				_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
				_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

				m_pPlayerTransform->SetLook(vPlayerLook);
			}

			else if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_1ST)
			{
				m_pPlayerTransform->Go_Right(_fTimeDelta, m_pPlayerNavigation);
			}

			if (pGameInstance->Get_DIKeyUp('S'))
			{
				_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(135.f));
				_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

				m_pPlayerTransform->SetLook(vPlayerLook);

				dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_runforward");
				dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RUN_R);
			}

			/* 공격 */
			else if (pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
			{
				m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());
				m_pPlayerTransform->Set_Speed(dynamic_cast<CPlayer*>(m_pPlayer)->GetWalkSpeed());

				dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_LATTACKB);
				dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_walkbwdattackleft");
			}
		}

		/* 공격 */
		else if (pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
		{
			m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());
			m_pPlayerTransform->Set_Speed(dynamic_cast<CPlayer*>(m_pPlayer)->GetWalkSpeed());

			dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_LATTACKB);
			dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_walkbwdattackleft");
		}

		else
		{
			m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());
		}
	}

	if (pGameInstance->Get_DIKeyUp('S'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_idle");
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_IDLE);
	}

	Safe_Release(pGameInstance);
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
