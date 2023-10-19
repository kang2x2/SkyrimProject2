#include "framework.h"
#include "StateManager_Player.h"

#include "State_Player.h"

#include "StatePlayer_Idle.h"
#include "StatePlayer_RunFoward.h"
#include "StatePlayer_RunBackward.h"
#include "StatePlayer_RunLeft.h"
#include "StatePlayer_RunRight.h"

#include "StatePlayerOH_LAttack.h"
#include "StatePlayerOH_RAttack.h"
#include "StatePlayerOH_PAttack.h"
#include "StatePlayerOH_RunPAttack.h"

CStateManager_Player::CStateManager_Player()
{
}

HRESULT CStateManager_Player::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform)
{
	if (_pPlayer == nullptr)
		return E_FAIL;

	/* Idle */
	CState_Player* pState = CStatePlayer_Idle::Create(_pPlayer, _pPlayerTransform);
	m_vecPlayerState.push_back(pState);

	/* Run */
	pState = CStatePlayer_RunFoward::Create(_pPlayer, _pPlayerTransform);
	m_vecPlayerState.push_back(pState);

	pState = CStatePlayer_RunBackward::Create(_pPlayer, _pPlayerTransform);
	m_vecPlayerState.push_back(pState);

	pState = CStatePlayer_RunLeft::Create(_pPlayer, _pPlayerTransform);
	m_vecPlayerState.push_back(pState);

	pState = CStatePlayer_RunRight::Create(_pPlayer, _pPlayerTransform);
	m_vecPlayerState.push_back(pState);

	/* Attack */
	pState = CStatePlayerOH_LAttack::Create(_pPlayer, _pPlayerTransform);
	m_vecPlayerState.push_back(pState);

	pState = CStatePlayerOH_RAttack::Create(_pPlayer, _pPlayerTransform);
	m_vecPlayerState.push_back(pState);

	pState = CStatePlayerOH_PAttack::Create(_pPlayer, _pPlayerTransform);
	m_vecPlayerState.push_back(pState);

	pState = CStatePlayerOH_RunPAttack::Create(_pPlayer, _pPlayerTransform);
	m_vecPlayerState.push_back(pState);
	
	m_pCurState = m_vecPlayerState.front();

	return S_OK;
}

CState_Player* CStateManager_Player::Get_State(PLAYERSTATE _eState)
{
	return m_vecPlayerState[_eState];
}

HRESULT CStateManager_Player::Set_State(PLAYERSTATE _eState)
{
	m_pCurState = m_vecPlayerState[_eState];

	return S_OK;
}

void CStateManager_Player::Update(_float _fTimeDelta)
{
	m_pCurState->Update(_fTimeDelta);
}

void CStateManager_Player::Late_Update()
{
	m_pCurState->Late_Update();
}

CStateManager_Player* CStateManager_Player::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform)
{
	CStateManager_Player* pInstance = new CStateManager_Player();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform)))
	{
		MSG_BOX("Fail Create : CStateManager_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateManager_Player::Free()
{
	__super::Free();

	for (auto& iter : m_vecPlayerState)
	{
		Safe_Release(iter);
	}
}
