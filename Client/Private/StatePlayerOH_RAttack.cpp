#include "framework.h"
#include "StatePlayerOH_RAttack.h"

#include "GameInstance.h"

#include "Player.h"
#include "Player_Weapon.h"

CStatePlayerOH_RAttack::CStatePlayerOH_RAttack()
{
}

HRESULT CStatePlayerOH_RAttack::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_RAttack::Update(_float _fTimeDelta)
{
	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_1ST)
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

	dynamic_cast<CPlayer*>(m_pPlayer)->CheckHit_Onehand(14, 16);
	
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CurFrameIndex() >= 22 &&
		!dynamic_cast<CPlayer*>(m_pPlayer)->Get_CurAnimationName("1hm_attackleft") &&
		pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_LATTACK);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_attackleft");
	}

	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CurFrameIndex() >= 22 &&
		!dynamic_cast<CPlayer*>(m_pPlayer)->Get_CurAnimationName("1hm_attackleft") &&
		pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_LATTACK);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_attackleft");
	}

	Safe_Release(pGameInstance);
}

void CStatePlayerOH_RAttack::Late_Update()
{
	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_IsAnimationFin())
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_IDLE);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_idle");
	}
}

CStatePlayerOH_RAttack* CStatePlayerOH_RAttack::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_RAttack* pInstance = new CStatePlayerOH_RAttack();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_RAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_RAttack::Free()
{
}
