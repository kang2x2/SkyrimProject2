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
	m_pWeaponCollider = dynamic_cast<CCollider*>(m_pMonster->Get_Part(CSkeever::PART_WEAPON)->Get_Component(TEXT("Com_Collider_OBB")));

	if (FAILED(__super::Initialize(_pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
		return E_FAIL;

	return S_OK;
}

void CState_Skeever::Update(_float _fTimeDelta)
{
	if (m_pMonster->GetHp() <= 0.f)
	{
		m_pMonster->Set_State(CSkeever::SKEEVER_DEAD);
		m_pMonster->Play_Animation(false, "bleedoutintro");
	}

}

void CState_Skeever::Late_Update()
{
}

void CState_Skeever::Free()
{
}
