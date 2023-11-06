#include "framework.h"
#include "StateManager_FalmerUE.h"

#include "State_FalmerUE.h"

#include "StateFalmerUE_Squat.h"
#include "StateFalmerUE_SquatOutro.h"
#include "StateFalmerUE_SquatIntro.h"

#include "StateFalmerUE_Warning.h"
#include "StateFalmerUE_Detection.h"
#include "StateFalmerUE_Return.h"

#include "StateFalmerUE_Chase.h"

#include "StateFalmerUE_RunAtk.h"

CStateManager_FalmerUE::CStateManager_FalmerUE()
{
}

HRESULT CStateManager_FalmerUE::Initialize(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	if (_pMonster == nullptr)
		return E_FAIL;

	/* Idle */
	CState_FalmerUE* pState = CStateFalmerUE_Squat::Create(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);
	m_vecMonsterState.push_back(pState);
	pState = CStateFalmerUE_SquatOutro::Create(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);
	m_vecMonsterState.push_back(pState);
	pState = CStateFalmerUE_SquatIntro::Create(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);
	m_vecMonsterState.push_back(pState);

	/* Warning */
	pState = CStateFalmerUE_Warning::Create(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);
	m_vecMonsterState.push_back(pState);

	/* Detection */
	pState = CStateFalmerUE_Detection::Create(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);
	m_vecMonsterState.push_back(pState);

	/* Return */
	pState = CStateFalmerUE_Return::Create(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);
	m_vecMonsterState.push_back(pState);

	/* Chase */
	pState = CStateFalmerUE_Chase::Create(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);
	m_vecMonsterState.push_back(pState);

	/* Attack */
	pState = CStateFalmerUE_RunAtk::Create(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);
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

CStateManager_FalmerUE* CStateManager_FalmerUE::Create(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateManager_FalmerUE* pInstance = new CStateManager_FalmerUE();

	if (FAILED(pInstance->Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
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

