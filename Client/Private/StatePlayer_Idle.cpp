#include "framework.h"
#include "StatePlayer_Idle.h"

#include "GameInstance.h"

#include "Player.h"


CStatePlayer_Idle::CStatePlayer_Idle()
{
}

HRESULT CStatePlayer_Idle::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayer_Idle::Update(_float _fTimeDelta)
{
	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_1ST)
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());
	
	Key_Input(_fTimeDelta);
}

void CStatePlayer_Idle::Late_Update()
{

}

void CStatePlayer_Idle::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* 이동 */
	if (pGameInstance->Get_DIKeyPress('W'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::UNEQUIP_RUN_F);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_runforward");
	}

	if (pGameInstance->Get_DIKeyPress('S'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::UNEQUIP_RUN_B);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_runbackward");
	}

	if (pGameInstance->Get_DIKeyPress('A'))
	{
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

		if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_3ST)
		{
			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-90.f));
			_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

			m_pPlayerTransform->SetLook(vPlayerLook);
		}

		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::UNEQUIP_RUN_L);
		
		// 분명히 필요하긴 할거야 이 함수가.
		//if(dynamic_cast<CPlayer*>(m_pPlayer)->Get_CurAnimationName("1hm_runforward"))
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_runforward");
	}

	if (pGameInstance->Get_DIKeyPress('D'))
	{
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());
		if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_3ST)
		{
			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(90.f));
			_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

			m_pPlayerTransform->SetLook(vPlayerLook);

		}
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_runforward");
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::UNEQUIP_RUN_R);
		
	}

	//if (pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
	//{
	//	dynamic_cast<CPlayer*>(m_pPlayer)->Set_PlayerEquipState(CPlayer::EQUIP_ONEHAND);
	//	dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_EQUIP);
	//	dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_equip");
	//}

	if (pGameInstance->Get_DIKeyDown(VK_LBUTTON))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_PlayerEquipState(CPlayer::EQUIP_ONEHAND);
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_EQUIP);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_equip");
	}

	Safe_Release(pGameInstance);

}

CStatePlayer_Idle* CStatePlayer_Idle::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayer_Idle* pInstance = new CStatePlayer_Idle();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayer_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayer_Idle::Free()
{
}
