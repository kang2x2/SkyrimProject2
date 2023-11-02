#include "framework.h"
#include "StateManager_Skeever.h"

#include "State_Monster.h"

#include "StateSkeever_Idle.h"

CStateManager_Skeever::CStateManager_Skeever()
{
}

CState_Monster* CStateManager_Skeever::Get_State(SKEEVERSTATE _eState)
{
	return nullptr;
}

HRESULT CStateManager_Skeever::Initialize(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation)
{
	if (_pMonster == nullptr)
		return E_FAIL;

	/* Idle */
	CState_Monster* pState = CStateSkeever_Idle::Create(_pMonster, _pMonsterTransform, _pMonsterNavigation);
	m_vecMonsterState.push_back(pState);

	m_pCurState = m_vecMonsterState.front();

	return S_OK;

}

HRESULT CStateManager_Skeever::Set_State(SKEEVERSTATE _eState)
{
	return E_NOTIMPL;
}

void CStateManager_Skeever::Update(_float _fTimeDelta)
{
}

void CStateManager_Skeever::Late_Update()
{
}

CStateManager_Skeever* CStateManager_Skeever::Create(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation)
{
	return nullptr;
}

void CStateManager_Skeever::Free()
{
}
