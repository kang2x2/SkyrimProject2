#include "framework.h"
#include "StatePlayerOH_RunFoward.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_RunFoward::CStatePlayerOH_RunFoward()
{
}

HRESULT CStatePlayerOH_RunFoward::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_RunFoward::Update(_float _fTimeDelta)
{
	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_1ST)
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

	Key_Input(_fTimeDelta);
}

void CStatePlayerOH_RunFoward::Late_Update()
{
}

void CStatePlayerOH_RunFoward::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyPress('W'))
	{
		m_pPlayerTransform->Go_Foward(_fTimeDelta, m_pPlayerNavigation);

		if (pGameInstance->Get_DIKeyPress('A'))
		{
			if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_3ST)
			{
				m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

				_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
				_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

				m_pPlayerTransform->SetLook(vPlayerLook);
			}

			else if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_1ST)
			{
				m_pPlayerTransform->Go_Left(_fTimeDelta, m_pPlayerNavigation);
			}

			if (pGameInstance->Get_DIKeyUp('W'))
			{
				if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_3ST)
				{
					_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
					_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

					m_pPlayerTransform->SetLook(vPlayerLook);
				}
				dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RUN_L);
			}

			/* 공격 */
			else if (pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
			{
				m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());
				m_pPlayerTransform->Set_Speed(dynamic_cast<CPlayer*>(m_pPlayer)->GetWalkSpeed());

				dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_LATTACKF);
				dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_walkfwdattackleft");
			}
		}

		else if (pGameInstance->Get_DIKeyPress('D'))
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
				m_pPlayerTransform->Go_Right(_fTimeDelta, m_pPlayerNavigation);
			}

			if (pGameInstance->Get_DIKeyUp('W'))
			{
				if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_3ST)
				{
					_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
					_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

					m_pPlayerTransform->SetLook(vPlayerLook);
				}

				dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RUN_R);
			}

			/* 공격 */
			else if (pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
			{
				m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());
				m_pPlayerTransform->Set_Speed(dynamic_cast<CPlayer*>(m_pPlayer)->GetWalkSpeed());

				dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_LATTACKF);
				dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_walkfwdattackleft");
			}
		}

		/* 공격 */
		else if (pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
		{
			m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());
			m_pPlayerTransform->Set_Speed(dynamic_cast<CPlayer*>(m_pPlayer)->GetWalkSpeed());

			dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_LATTACKF);
			dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_walkfwdattackleft");
		}

		else
		{
			m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());
		}

	}

	if (pGameInstance->Get_DIKeyUp('W'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_IDLE);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_idle");
	}

	Safe_Release(pGameInstance);



	//CGameInstance* pGameInstance = CGameInstance::GetInstance();
	//Safe_AddRef(pGameInstance);
	//
	//if (pGameInstance->Get_DIKeyPress('W'))
	//{
	//	m_pPlayerTransform->Go_Foward(_fTimeDelta, m_pPlayerNavigation);
	//
	//	if (pGameInstance->Get_DIKeyPress('A'))
	//	{
	//		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
	//		_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));
	//
	//		m_pPlayerTransform->SetLook(vPlayerLook);
	//		m_pPlayerTransform->Go_Left(_fTimeDelta, m_pPlayerNavigation);
	//
	//		if (pGameInstance->Get_DIKeyUp('W'))
	//		{
	//			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
	//			_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));
	//
	//			m_pPlayerTransform->SetLook(vPlayerLook);
	//
	//			dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RUN_L);
	//		}
	//	}
	//
	//	else if (pGameInstance->Get_DIKeyPress('D'))
	//	{
	//		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
	//		_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));
	//
	//		m_pPlayerTransform->SetLook(vPlayerLook);
	//		m_pPlayerTransform->Go_Right(_fTimeDelta, m_pPlayerNavigation);
	//
	//		if (pGameInstance->Get_DIKeyUp('W'))
	//		{
	//			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
	//			_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));
	//
	//			m_pPlayerTransform->SetLook(vPlayerLook);
	//
	//			dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RUN_R);
	//		}
	//	}
	//
	//	else if (pGameInstance->Get_DIKeyDown('R'))
	//	{
	//		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RUNPOWERATTACK);
	//		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_attackpowerforwardsprint");
	//	}
	//}
	//
	//if (pGameInstance->Get_DIKeyUp('W'))
	//{
	//	dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_IDLE);
	//	dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_idle");
	//}
	//
	//Safe_Release(pGameInstance);
}

CStatePlayerOH_RunFoward* CStatePlayerOH_RunFoward::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_RunFoward* pInstance = new CStatePlayerOH_RunFoward();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_RunFoward");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_RunFoward::Free()
{
}
