#include "framework.h"
#include "StatePlayerOH_RAttackB.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_RAttackB::CStatePlayerOH_RAttackB()
{
}

HRESULT CStatePlayerOH_RAttackB::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_RAttackB::Update(_float _fTimeDelta)
{
	Key_Input(_fTimeDelta);

	m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CurFrameIndex() >= 25 &&
		dynamic_cast<CPlayer*>(m_pPlayer)->Get_CurAnimationName("1hm_walkbwdattackright") &&
		pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_LATTACKB);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_walkbwdattackleft");
	}

	Safe_Release(pGameInstance);
}

void CStatePlayerOH_RAttackB::Late_Update()
{
	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_IsAnimationFin())
	{
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

		m_pPlayerTransform->Set_Speed(dynamic_cast<CPlayer*>(m_pPlayer)->GetRunSpeed());

		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_IDLE);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_idle");
	}
}

void CStatePlayerOH_RAttackB::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyPress('W'))
	{
		m_pPlayerTransform->Go_Foward(_fTimeDelta, m_pPlayerNavigation);
	}

	else if (pGameInstance->Get_DIKeyPress('S'))
	{
		m_pPlayerTransform->Go_Backward(_fTimeDelta, m_pPlayerNavigation);
	}

	Safe_Release(pGameInstance);

}

CStatePlayerOH_RAttackB* CStatePlayerOH_RAttackB::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_RAttackB* pInstance = new CStatePlayerOH_RAttackB();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_RAttackB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_RAttackB::Free()
{
}
