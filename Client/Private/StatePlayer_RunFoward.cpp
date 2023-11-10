#include "framework.h"
#include "StatePlayer_RunFoward.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayer_RunFoward::CStatePlayer_RunFoward()
{
}

HRESULT CStatePlayer_RunFoward::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayer_RunFoward::Update(_float _fTimeDelta)
{
	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_1ST)
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

	Key_Input(_fTimeDelta);
}

void CStatePlayer_RunFoward::Late_Update()
{
}

void CStatePlayer_RunFoward::Key_Input(_float _fTimeDelta)
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

				dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::UNEQUIP_RUN_L);
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

				dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::UNEQUIP_RUN_R);
			}
		}

		else
		{
			m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());
		}

	}

	if (pGameInstance->Get_DIKeyUp('W'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::UNEQUIP_IDLE);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_idle");
	}

	Safe_Release(pGameInstance);
}

CStatePlayer_RunFoward* CStatePlayer_RunFoward::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayer_RunFoward* pInstance = new CStatePlayer_RunFoward();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayer_RunFoward");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayer_RunFoward::Free()
{
}
