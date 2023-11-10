#include "framework.h"
#include "StatePlayer_RunRight.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayer_RunRight::CStatePlayer_RunRight()
{
}

HRESULT CStatePlayer_RunRight::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayer_RunRight::Update(_float _fTimeDelta)
{
	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_1ST)
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

	Key_Input(_fTimeDelta);
}

void CStatePlayer_RunRight::Late_Update()
{
}

void CStatePlayer_RunRight::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyPress('D'))
	{
		if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_3ST)
		{
			m_pPlayerTransform->Go_Foward(_fTimeDelta, m_pPlayerNavigation);
		}
		else if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_1ST)
		{
			m_pPlayerTransform->Go_Right(_fTimeDelta, m_pPlayerNavigation);
		}

		if (pGameInstance->Get_DIKeyPress('W'))
		{
			dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::UNEQUIP_RUN_F);
		}
	}

	if (pGameInstance->Get_DIKeyUp('D'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::UNEQUIP_IDLE);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_idle");
	}

	Safe_Release(pGameInstance);
}

CStatePlayer_RunRight* CStatePlayer_RunRight::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayer_RunRight* pInstance = new CStatePlayer_RunRight();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayer_RunRight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayer_RunRight::Free()
{
}
