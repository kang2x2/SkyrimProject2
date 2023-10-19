#include "framework.h"
#include "StatePlayerOH_LAttack.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_LAttack::CStatePlayerOH_LAttack()
{
}

HRESULT CStatePlayerOH_LAttack::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform)
{
	__super::Initialize(_pPlayer, _pPlayerTransform);

	return S_OK;
}

void CStatePlayerOH_LAttack::Update(_float _fTimeDelta)
{
	dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "LeftAttack");
}

void CStatePlayerOH_LAttack::Late_Update()
{
	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_IsAnimationFin())
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_IDLE);
}

CStatePlayerOH_LAttack* CStatePlayerOH_LAttack::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform)
{
	CStatePlayerOH_LAttack* pInstance = new CStatePlayerOH_LAttack();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_LAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_LAttack::Free()
{
}
