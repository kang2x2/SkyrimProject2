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
	m_pPlayerTransform->Go_Backward(_fTimeDelta);
}

void CStatePlayer_RunBackward::Late_Update()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyUp('S'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "Idle");
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_IDLE);
	}

	if (pGameInstance->Get_DIKeyPress('A'))
		m_pPlayerTransform->Fix_Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.0f));

	if (pGameInstance->Get_DIKeyPress('D'))
		m_pPlayerTransform->Fix_Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-45.0f));

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
