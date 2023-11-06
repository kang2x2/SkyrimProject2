#include "State_Monster.h"
#include "..\Public\State_Monster.h"

CState_Monster::CState_Monster()
{
}

HRESULT CState_Monster::Initialize(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation)
{
	if (_pMonster == nullptr)
		return E_FAIL;

	m_pMonster = _pMonster;
	m_pMonsterTransform = _pMonsterTransform;
	m_pMonsterNavigation = _pMonsterNavigation;

	return S_OK;

}

void CState_Monster::Update(_float _fTimeDelta)
{
}

void CState_Monster::Late_Update()
{
}

void CState_Monster::Free()
{
	__super::Free();
}
