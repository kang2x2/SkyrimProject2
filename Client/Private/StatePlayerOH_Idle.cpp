#include "framework.h"
#include "StatePlayerOH_Idle.h"

#include "GameInstance.h"

#include "Player.h"


CStatePlayerOH_Idle::CStatePlayerOH_Idle()
{
}

HRESULT CStatePlayerOH_Idle::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_Idle::Update(_float _fTimeDelta)
{
	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_1ST)
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

	Key_Input(_fTimeDelta);
}

void CStatePlayerOH_Idle::Late_Update()
{

}

void CStatePlayerOH_Idle::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* ÀÌµ¿ */
	if (pGameInstance->Get_DIKeyPress('W'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RUN_F);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_runforward");
	}

	else if (pGameInstance->Get_DIKeyPress('S'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RUN_B);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_runbackward");
	}

	else if (pGameInstance->Get_DIKeyPress('A'))
	{
		if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_3ST)
		{
			m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-90.f));
			_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

			m_pPlayerTransform->SetLook(vPlayerLook);

			dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_runforward");
		}
		else if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_1ST)
		{
			dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_1stp_turnleft");
		}

		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RUN_L);
		
	}

	else if (pGameInstance->Get_DIKeyPress('D'))
	{
		if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_3ST)
		{
			m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(90.f));
			_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

			m_pPlayerTransform->SetLook(vPlayerLook);

			dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_runforward");
		}
		else if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_1ST)
		{
			dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_1stp_turnright");
		}

		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RUN_R);

	}

	else if (pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_LATTACK);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_attackleft");
	}

	else if (pGameInstance->Get_DIKeyDown('R'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_PlayerEquipState(CPlayer::EQUIP_UNEQUIP);

		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_UNEQUIP);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_unequip");
	}

	if (pGameInstance->Get_DIKeyPress(VK_RBUTTON))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_BLOCK);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_blockidle");
	}

	Safe_Release(pGameInstance);

}

CStatePlayerOH_Idle* CStatePlayerOH_Idle::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_Idle* pInstance = new CStatePlayerOH_Idle();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_Idle::Free()
{
}
