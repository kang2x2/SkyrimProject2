#include "State_FalmerUE.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"

CState_FalmerUE::CState_FalmerUE()
{
}

HRESULT CState_FalmerUE::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	if (_pPlayer == nullptr)
		return E_FAIL;

	m_pMonster = dynamic_cast<CFalmer_UnEquip*>(_pMonster);
	m_pWeaponCollider = dynamic_cast<CCollider*>(m_pMonster->Get_Part(CFalmer_UnEquip::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB")));

	if(FAILED(__super::Initialize(_pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
		return E_FAIL;

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
