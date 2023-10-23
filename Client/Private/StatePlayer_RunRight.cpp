#include "framework.h"
#include "StatePlayer_RunRight.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayer_RunRight::CStatePlayer_RunRight()
{
}

HRESULT CStatePlayer_RunRight::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform)
{
	__super::Initialize(_pPlayer, _pPlayerTransform);

	return S_OK;
}

void CStatePlayer_RunRight::Update(_float _fTimeDelta)
{
	m_pPlayerTransform->Go_Right(_fTimeDelta);
}

void CStatePlayer_RunRight::Late_Update()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyUp('D'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_idle");
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_IDLE);
	}

	Safe_Release(pGameInstance);
}

CStatePlayer_RunRight* CStatePlayer_RunRight::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform)
{
	CStatePlayer_RunRight* pInstance = new CStatePlayer_RunRight();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform)))
	{
		MSG_BOX("Fail Create : CStatePlayer_RunRight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayer_RunRight::Free()
{
}
