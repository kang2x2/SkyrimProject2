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
	m_pPlayerTransform->Go_Left(_fTimeDelta);
	dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "RunLeft");
}

void CStatePlayer_RunLeft::Late_Update()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyUp('A'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_IDLE);
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
