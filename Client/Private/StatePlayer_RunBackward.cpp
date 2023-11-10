#include "framework.h"
#include "StatePlayer_RunBackward.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayer_RunBackward::CStatePlayer_RunBackward()
{
}

HRESULT CStatePlayer_RunBackward::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayer_RunBackward::Update(_float _fTimeDelta)
{
	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_1ST)
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

	Key_Input(_fTimeDelta);
}

void CStatePlayer_RunBackward::Late_Update()
{
}

void CStatePlayer_RunBackward::Key_Input(_float _fTimeDelta)
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

			if (pGameInstance->Get_DIKeyUp('S'))
			{
				_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(135.f));
				_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

				m_pPlayerTransform->SetLook(vPlayerLook);

				dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_runforward");
				dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::UNEQUIP_RUN_L);
			}
		}

		else if (pGameInstance->Get_DIKeyPress('D'))
		{
			m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
			_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

			m_pPlayerTransform->SetLook(vPlayerLook);

			if (pGameInstance->Get_DIKeyUp('S'))
			{
				_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(135.f));
				_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

				m_pPlayerTransform->SetLook(vPlayerLook);

				dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_runforward");
				dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::UNEQUIP_RUN_R);
			}
		}

		else
		{
			m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());
		}
	}

	if (pGameInstance->Get_DIKeyUp('S'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_idle");
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::UNEQUIP_IDLE);
	}

	Safe_Release(pGameInstance);
}

CStatePlayer_RunBackward* CStatePlayer_RunBackward::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayer_RunBackward* pInstance = new CStatePlayer_RunBackward();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayer_RunBackward");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayer_RunBackward::Free()
{
}
