#include "framework.h"
#include "StatePlayerOH_PAttack.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_PAttack::CStatePlayerOH_PAttack()
{
}

HRESULT CStatePlayerOH_PAttack::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_PAttack::Update(_float _fTimeDelta)
{
	__super::Key_Input();
}

void CStatePlayerOH_PAttack::Late_Update()
{
	if (m_pPlayer->Get_IsAnimationFin())
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_IDLE);
		m_pPlayer->Play_Animation(true, "1hm_idle");
	}
}

CStatePlayerOH_PAttack* CStatePlayerOH_PAttack::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_PAttack* pInstance = new CStatePlayerOH_PAttack();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_PAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_PAttack::Free()
{
}
