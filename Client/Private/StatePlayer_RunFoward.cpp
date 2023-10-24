#include "framework.h"
#include "StatePlayer_RunFoward.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayer_RunFoward::CStatePlayer_RunFoward()
{
}

HRESULT CStatePlayer_RunFoward::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform)
{
	__super::Initialize(_pPlayer, _pPlayerTransform);

	return S_OK;
}

void CStatePlayer_RunFoward::Update(_float _fTimeDelta)
{
	m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());
	m_pPlayerTransform->Go_Foward(_fTimeDelta);

	Key_Input(_fTimeDelta);
}

void CStatePlayer_RunFoward::Late_Update()
{
}

void CStatePlayer_RunFoward::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	if (pGameInstance->Get_DIKeyUp('W'))
	{
		if (pGameInstance->Get_DIKeyPress('A'))
		{
			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
			_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

			m_pPlayerTransform->SetLook(vPlayerLook);

			dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_RUN_L);
		}

		else if (pGameInstance->Get_DIKeyPress('D'))
		{
			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
			_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

			m_pPlayerTransform->SetLook(vPlayerLook);

			dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_RUN_R);
		}

		else
		{
			dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_idle");
			dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_IDLE);
		}
	}

	if (pGameInstance->Get_DIKeyPress('A'))
	{
		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
		_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));
		
		m_pPlayerTransform->SetLook(vPlayerLook);
		m_pPlayerTransform->Go_Left(_fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyPress('D'))
	{
		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
		_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

		m_pPlayerTransform->SetLook(vPlayerLook);
		m_pPlayerTransform->Go_Right(_fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyDown('R'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_RUNPOWERATTACK);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_attackpowerforwardsprint");
	}

	Safe_Release(pGameInstance);
}

CStatePlayer_RunFoward* CStatePlayer_RunFoward::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform)
{
	CStatePlayer_RunFoward* pInstance = new CStatePlayer_RunFoward();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform)))
	{
		MSG_BOX("Fail Create : CStatePlayer_RunFoward");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayer_RunFoward::Free()
{
}
