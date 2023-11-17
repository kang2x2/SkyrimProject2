#include "State_Skeever.h"

#include "GameInstance.h"

#include "Skeever.h"
#include "Player.h"


CState_Skeever::CState_Skeever()
{
}

HRESULT CState_Skeever::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	if (_pPlayer == nullptr)
		return E_FAIL;

	m_pMonster = dynamic_cast<CSkeever*>(_pMonster);
	m_pPlayer = dynamic_cast<CPlayer*>(_pPlayer);
	m_pMonsterTransform = _pMonsterTransform;
	m_pMonsterNavigation = _pMonsterNavigation;
	m_pVecCollider = _pVecColCom;

	return S_OK;
}

void CState_Skeever::Update(_float _fTimeDelta)
{
}

void CState_Skeever::Late_Update()
{
}

void CState_Skeever::Free()
{
}
