#include "framework.h"
#include "StateManager_FalmerUE.h"

#include "State_Monster.h"

#include "StateFalmerUE_Idle.h"

CStateManager_FalmerUE::CStateManager_FalmerUE()
{
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

CState_Monster* CStateManager_FalmerUE::Get_State(CFalmer_UnEquip::CFalmer_UnEquip::FALMERUE_STATE _eState)
{
	return m_vecMonsterState[_eState];
}

HRESULT CStateManager_FalmerUE::Set_State(CFalmer_UnEquip::FALMERUE_STATE _eState)
{
	m_pCurState = m_vecMonsterState[_eState];

	return S_OK;
}

void CStateManager_FalmerUE::Update(_float _fTimeDelta)
{
	m_pCurState->Update(_fTimeDelta);
}

void CStateManager_FalmerUE::Late_Update()
{
	m_pCurState->Late_Update();
}

CStateManager_FalmerUE* CStateManager_FalmerUE::Create(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation)
{
	CStateManager_FalmerUE* pInstance = new CStateManager_FalmerUE();

	if (FAILED(pInstance->Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation)))
	{
		MSG_BOX("Fail Create : CStateManager_FalmerUE");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateManager_FalmerUE::Free()
{
	__super::Free();

	for (auto& iter : m_vecMonsterState)
	{
		Safe_Release(iter);
	}
}

