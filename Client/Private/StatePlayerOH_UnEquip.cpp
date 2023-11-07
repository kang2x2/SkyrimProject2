#include "framework.h"
#include "StatePlayerOH_UnEquip.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_UnEquip::CStatePlayerOH_UnEquip()
{
}

HRESULT CStatePlayerOH_UnEquip::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_UnEquip::Update(_float _fTimeDelta)
{
	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CurFrameIndex() >= 30 &&
		dynamic_cast<CPlayer*>(m_pPlayer)->Get_CurAnimationName("1hm_unequip") &&
		strcmp(dynamic_cast<CPlayer*>(m_pPlayer)->Get_CurSocketBonName(), "WeaponSword"))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_SoketBone("WeaponSword");
	}
}

void CStatePlayerOH_UnEquip::Late_Update()
{
	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_IsAnimationFin() &&
		dynamic_cast<CPlayer*>(m_pPlayer)->Get_CurAnimationName("1hm_unequip"))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_idle");
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::UNEQUIP_IDLE);
	}
}

CStatePlayerOH_UnEquip* CStatePlayerOH_UnEquip::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_UnEquip* pInstance = new CStatePlayerOH_UnEquip();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_UnEquip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_UnEquip::Free()
{
}
