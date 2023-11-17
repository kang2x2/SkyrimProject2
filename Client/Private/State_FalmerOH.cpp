#include "State_FalmerOH.h"

#include "GameInstance.h"

#include "Falmer_OneHand.h"
#include "Player.h"

CState_FalmerOH::CState_FalmerOH()
{
}

HRESULT CState_FalmerOH::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	if (_pPlayer == nullptr)
		return E_FAIL;

	m_pMonster = dynamic_cast<CFalmer_OneHand*>(_pMonster);
	m_pPlayer = dynamic_cast<CPlayer*>(_pPlayer);
	m_pMonsterTransform = _pMonsterTransform;
	m_pMonsterNavigation = _pMonsterNavigation;
	m_pVecCollider = _pVecColCom;

	return S_OK;
}

void CState_FalmerOH::Update(_float _fTimeDelta)
{
}

void CState_FalmerOH::Late_Update()
{
}

void CState_FalmerOH::Free()
{
}
