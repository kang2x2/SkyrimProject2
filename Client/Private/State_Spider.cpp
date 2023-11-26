#include "framework.h"
#include "State_Spider.h"

#include "GameInstance.h"

#include "Spider.h"

CState_Spider::CState_Spider()
{
}

HRESULT CState_Spider::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	if (_pPlayer == nullptr)
		return E_FAIL;

	m_pMonster = dynamic_cast<CSpider*>(_pMonster);
	m_pWeaponCollider = dynamic_cast<CCollider*>(m_pMonster->Get_Part(CSpider::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB")));

	if (FAILED(__super::Initialize(_pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
		return E_FAIL;

	return S_OK;
}

void CState_Spider::Update(_float _fTimeDelta)
{
}

void CState_Spider::Late_Update()
{
}

void CState_Spider::Free()
{
}
