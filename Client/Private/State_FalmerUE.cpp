#include "State_FalmerUE.h"

#include "GameInstance.h"

CState_FalmerUE::CState_FalmerUE()
{
}

HRESULT CState_FalmerUE::Initialize(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation);

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
