#include "framework.h"
#include "StatePlayer_RunBackward.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayer_RunBackward::CStatePlayer_RunBackward()
{
}

HRESULT CStatePlayer_RunBackward::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform)
{
	__super::Initialize(_pPlayer, _pPlayerTransform);

	return S_OK;
}

void CStatePlayer_RunBackward::Update(_float _fTimeDelta)
{
	m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

	m_pPlayerTransform->Go_Backward(_fTimeDelta);

	Key_Input(_fTimeDelta);
}

void CStatePlayer_RunBackward::Late_Update()
{
}

void CStatePlayer_RunBackward::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 여기 상태제어 이어서

	if (pGameInstance->Get_DIKeyUp('S'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_idle");
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_IDLE);
	}

	if (pGameInstance->Get_DIKeyPress('A'))
	{
		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
		_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

		m_pPlayerTransform->SetLook(vPlayerLook);
		m_pPlayerTransform->Go_Left(_fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyPress('D'))
	{
		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
		_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

		m_pPlayerTransform->SetLook(vPlayerLook);
		m_pPlayerTransform->Go_Right(_fTimeDelta);
	}

	Safe_Release(pGameInstance);
}

CStatePlayer_RunBackward* CStatePlayer_RunBackward::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform)
{
	CStatePlayer_RunBackward* pInstance = new CStatePlayer_RunBackward();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform)))
	{
		MSG_BOX("Fail Create : CStatePlayer_RunBackward");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayer_RunBackward::Free()
{
}
