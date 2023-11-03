#include "framework.h"
#include "StatePlayerOH_RunBackward.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_RunBackward::CStatePlayerOH_RunBackward()
{
}

HRESULT CStatePlayerOH_RunBackward::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_RunBackward::Update(_float _fTimeDelta)
{
	m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

	m_pPlayerTransform->Go_Backward(_fTimeDelta, m_pPlayerNavigation);

	Key_Input(_fTimeDelta);
}

void CStatePlayerOH_RunBackward::Late_Update()
{
}

void CStatePlayerOH_RunBackward::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 여기 상태제어 이어서

	if (pGameInstance->Get_DIKeyUp('S'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_idle");
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_IDLE);
	}

	if (pGameInstance->Get_DIKeyPress('A'))
	{
		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
		_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

		m_pPlayerTransform->SetLook(vPlayerLook);
		m_pPlayerTransform->Go_Left(_fTimeDelta, m_pPlayerNavigation);
	}

	if (pGameInstance->Get_DIKeyPress('D'))
	{
		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
		_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

		m_pPlayerTransform->SetLook(vPlayerLook);
		m_pPlayerTransform->Go_Right(_fTimeDelta, m_pPlayerNavigation);
	}

	Safe_Release(pGameInstance);
}

CStatePlayerOH_RunBackward* CStatePlayerOH_RunBackward::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_RunBackward* pInstance = new CStatePlayerOH_RunBackward();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_RunBackward");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_RunBackward::Free()
{
}
