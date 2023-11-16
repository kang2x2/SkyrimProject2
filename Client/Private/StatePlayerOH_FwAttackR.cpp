#include "framework.h"
#include "StatePlayerOH_FwAttackR.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_FwAttackR::CStatePlayerOH_FwAttackR()
{
}

HRESULT CStatePlayerOH_FwAttackR::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_FwAttackR::Update(_float _fTimeDelta)
{
	if (m_pPlayer->Get_CamMode() == CPlayer::CAM_3ST)
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

	Key_Input(_fTimeDelta);

	__super::Key_Input(_fTimeDelta);
}

void CStatePlayerOH_FwAttackR::Late_Update()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_pPlayer->Get_CurFrameIndex() >= 20 &&
		!strcmp(m_pPlayer->Get_CurAnimationName().c_str(), "1hm_walkfwdattackright") &&
		pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_FWATTACKL);
		m_pPlayer->Play_Animation(false, "1hm_walkfwdattackleft");
	}

	Safe_Release(pGameInstance);

	if (m_pPlayer->Get_IsAnimationFin())
	{
		m_pPlayerTransform->Set_Speed(m_pPlayer->GetRunSpeed());

		m_pPlayer->Set_State(CPlayer::ONEHAND_IDLE);
		m_pPlayer->Play_Animation(true, "1hm_idle");
	}
}

void CStatePlayerOH_FwAttackR::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyPress('W'))
	{
		m_pPlayerTransform->Go_Foward(_fTimeDelta, m_pPlayerNavigation);
	}
	else if (pGameInstance->Get_DIKeyPress('A'))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_LWATTACKR);
		m_pPlayer->Play_Animation(false, "1hm_walkleftattackright", m_pPlayer->Get_CurFrameIndex());
	}
	else if (pGameInstance->Get_DIKeyPress('D'))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_RWATTACKR);
		m_pPlayer->Play_Animation(false, "1hm_walkrightattackright", m_pPlayer->Get_CurFrameIndex());
	}
	else if (pGameInstance->Get_DIKeyPress('S'))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_BWATTACKR);
		m_pPlayer->Play_Animation(false, "1hm_walkbwdattackright", m_pPlayer->Get_CurFrameIndex());
	}

	Safe_Release(pGameInstance);

}

CStatePlayerOH_FwAttackR* CStatePlayerOH_FwAttackR::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_FwAttackR* pInstance = new CStatePlayerOH_FwAttackR();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_FwAttackR");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_FwAttackR::Free()
{
}
