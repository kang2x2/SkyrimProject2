#include "State_FalmerUE.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"
#include "Player.h"

CState_FalmerUE::CState_FalmerUE()
{
}

HRESULT CState_FalmerUE::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	if (_pPlayer == nullptr)
		return E_FAIL;

	m_pMonster = dynamic_cast<CFalmer_UnEquip*>(_pMonster);
	m_pPlayer = dynamic_cast<CPlayer*>(_pPlayer);
	m_pMonsterTransform = _pMonsterTransform;
	m_pMonsterNavigation = _pMonsterNavigation;
	m_pVecCollider = _pVecColCom;

	return S_OK;
}

void CState_FalmerUE::Update(_float _fTimeDelta)
{
}

void CState_FalmerUE::Late_Update()
{
}

void CState_FalmerUE::Free()
{
}
