#include "framework.h"
#include "StatePlayerOH_RAttack.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_RAttack::CStatePlayerOH_RAttack()
{
}

HRESULT CStatePlayerOH_RAttack::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform)
{
	__super::Initialize(_pPlayer, _pPlayerTransform);

	return S_OK;
}

void CStatePlayerOH_RAttack::Update(_float _fTimeDelta)
{
}

void CStatePlayerOH_RAttack::Late_Update()
{
	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_IsAnimationFin())
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "Idle");
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_IDLE);
	}
}

CStatePlayerOH_RAttack* CStatePlayerOH_RAttack::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform)
{
	CStatePlayerOH_RAttack* pInstance = new CStatePlayerOH_RAttack();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_RAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_RAttack::Free()
{
}
