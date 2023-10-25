#include "framework.h"
#include "StatePlayer_RunLeft.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayer_RunLeft::CStatePlayer_RunLeft()
{
}

HRESULT CStatePlayer_RunLeft::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform)
{
	__super::Initialize(_pPlayer, _pPlayerTransform);

	return S_OK;
}

void CStatePlayer_RunLeft::Update(_float _fTimeDelta)
{
	m_pPlayerTransform->Go_Foward(_fTimeDelta);

	Key_Input(_fTimeDelta);
}

void CStatePlayer_RunLeft::Late_Update()
{
}

void CStatePlayer_RunLeft::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyUp('A'))
	{
		if (pGameInstance->Get_DIKeyPress('W'))
		{
			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
			_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

			m_pPlayerTransform->SetLook(vPlayerLook);

			dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_RUN_F);
		}
		else
		{
			// m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());
			dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_idle");
			dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_IDLE);
		}
	}

	else if (pGameInstance->Get_DIKeyDown('W'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_RUN_F);
	}
	else if (pGameInstance->Get_DIKeyDown('S'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_RUN_B);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_runbackward");
	}

	Safe_Release(pGameInstance);
}

CStatePlayer_RunLeft* CStatePlayer_RunLeft::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform)
{
	CStatePlayer_RunLeft* pInstance = new CStatePlayer_RunLeft();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform)))
	{
		MSG_BOX("Fail Create : CStatePlayer_RunLeft");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayer_RunLeft::Free()
{
}
