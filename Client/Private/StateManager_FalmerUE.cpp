#include "framework.h"
#include "StateManager_FalmerUE.h"

#include "State_Monster.h"

#include "StateFalmerUE_Idle.h"

CStateManager_FalmerUE::CStateManager_FalmerUE()
{
}

CState_Monster* CStateManager_FalmerUE::Get_State(SKEEVERSTATE _eState)
{
	return nullptr;
}

HRESULT CStateManager_FalmerUE::Initialize(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation)
{
	if (_pMonster == nullptr)
		return E_FAIL;

	/* Idle */
	CState_Monster* pState = CStateFalmerUE_Idle::Create(_pMonster, _pMonsterTransform, _pMonsterNavigation);
	m_vecMonsterState.push_back(pState);

	m_pCurState = m_vecMonsterState.front();

	return S_OK;

}

HRESULT CStateManager_FalmerUE::Set_State(SKEEVERSTATE _eState)
{
	return S_OK;
}

void CStateManager_FalmerUE::Update(_float _fTimeDelta)
{
}

void CStateManager_FalmerUE::Late_Update()
{
}

CStateManager_FalmerUE* CStateManager_FalmerUE::Create(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation)
{
	return nullptr;
}

void CStateManager_FalmerUE::Free()
{
}

