#include "framework.h"
#include "StatePlayer_RunLeft.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayer_RunLeft::CStatePlayer_RunLeft()
{
}

HRESULT CStatePlayer_RunLeft::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayer_RunLeft::Update(_float _fTimeDelta)
{
	Key_Input(_fTimeDelta);
}

void CStatePlayer_RunLeft::Late_Update()
{
}

void CStatePlayer_RunLeft::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyPress('A'))
	{
		m_pPlayerTransform->Go_Foward(_fTimeDelta, m_pPlayerNavigation);
	
		if (pGameInstance->Get_DIKeyPress('W'))
		{
			dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::UNEQUIP_RUN_F);
		}
	}

	if (pGameInstance->Get_DIKeyUp('A'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::UNEQUIP_IDLE);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_idle");
	}

	Safe_Release(pGameInstance);
}

CStatePlayer_RunLeft* CStatePlayer_RunLeft::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayer_RunLeft* pInstance = new CStatePlayer_RunLeft();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayer_RunLeft");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayer_RunLeft::Free()
{
}
