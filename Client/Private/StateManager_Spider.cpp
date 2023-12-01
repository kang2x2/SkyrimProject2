#include "framework.h"
#include "StateManager_Spider.h"

#include "State_Spider.h"

#include "StateSpider_Idle.h"
#include "StateSpider_GetUp.h"
#include "StateSpider_Chase.h"
#include "StateSpider_Spit.h"

CStateManager_Spider::CStateManager_Spider()
{
}

HRESULT CStateManager_Spider::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	if (_pMonster == nullptr)
		return E_FAIL;

	/* Idle */
	CState_Spider* pState = CStateSpider_Idle::Create(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);
	m_vecMonsterState.push_back(pState);

	/* GetUp */
	pState = CStateSpider_GetUp::Create(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);
	m_vecMonsterState.push_back(pState);

	/* Chase */
	pState = CStateSpider_Chase::Create(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);
	m_vecMonsterState.push_back(pState);

	/* Spit */
	pState = CStateSpider_Spit::Create(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);
	m_vecMonsterState.push_back(pState);

	m_pCurState = m_vecMonsterState.front();

	return S_OK;
}

CState_Spider* CStateManager_Spider::Get_State(CSpider::SPIDER_STATE _eState)
{
	return m_vecMonsterState[_eState];
}

HRESULT CStateManager_Spider::Set_State(CSpider::SPIDER_STATE _eState)
{
	m_pCurState = nullptr;
	m_pCurState = m_vecMonsterState[_eState];

	return S_OK;
}

void CStateManager_Spider::Update(_float _fTimeDelta)
{
	m_pCurState->Update(_fTimeDelta);
}

void CStateManager_Spider::Late_Update()
{
	m_pCurState->Late_Update();
}

CStateManager_Spider* CStateManager_Spider::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateManager_Spider* pInstance = new CStateManager_Spider();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateManager_Spider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateManager_Spider::Free()
{
	__super::Free();

	for (auto& iter : m_vecMonsterState)
	{
		Safe_Release(iter);
	}
}

