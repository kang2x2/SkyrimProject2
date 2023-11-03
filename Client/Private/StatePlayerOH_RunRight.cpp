#include "framework.h"
#include "StatePlayerOH_RunRight.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_RunRight::CStatePlayerOH_RunRight()
{
}

HRESULT CStatePlayerOH_RunRight::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_RunRight::Update(_float _fTimeDelta)
{
	m_pPlayerTransform->Go_Foward(_fTimeDelta, m_pPlayerNavigation);

	Key_Input(_fTimeDelta);
}

void CStatePlayerOH_RunRight::Late_Update()
{
}

void CStatePlayerOH_RunRight::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyUp('D'))
	{
		if (pGameInstance->Get_DIKeyPress('W'))
		{
			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
			_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

			m_pPlayerTransform->SetLook(vPlayerLook);

			dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RUN_F);
		}
		else
		{
			// m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());
			dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_idle");
			dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_IDLE);
		}
	}

	else if (pGameInstance->Get_DIKeyDown('W'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RUN_F);
	}
	else if (pGameInstance->Get_DIKeyDown('S'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RUN_B);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_runbackward");
	}

	Safe_Release(pGameInstance);
}

CStatePlayerOH_RunRight* CStatePlayerOH_RunRight::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_RunRight* pInstance = new CStatePlayerOH_RunRight();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_RunRight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_RunRight::Free()
{
}
