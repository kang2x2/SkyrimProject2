#include "framework.h"
#include "StatePlayerOH_FwAttackL.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_FwAttackL::CStatePlayerOH_FwAttackL()
{
}

HRESULT CStatePlayerOH_FwAttackL::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_FwAttackL::Update(_float _fTimeDelta)
{
	if (m_pPlayer->Get_CamMode() == CPlayer::CAM_3ST)
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

	Key_Input(_fTimeDelta);

	__super::Key_Input(_fTimeDelta);
}

void CStatePlayerOH_FwAttackL::Late_Update()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_pPlayer->Get_CurFrameIndex() >= 20 &&
		!strcmp(m_pPlayer->Get_CurAnimationName().c_str(), "1hm_walkfwdattackleft") &&
		pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_FWATTACKR);
		m_pPlayer->Play_Animation_All(false, "1hm_walkfwdattackright");
	}

	Safe_Release(pGameInstance);

	if (m_pPlayer->Get_IsAnimationFin())
	{
		m_pPlayerTransform->Set_Speed(m_pPlayer->GetRunSpeed());

		m_pPlayer->Set_State(CPlayer::ONEHAND_IDLE);
		m_pPlayer->Play_Animation_All(true, "1hm_idle");
	}
}

void CStatePlayerOH_FwAttackL::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyPress('W'))
	{
		m_pPlayerTransform->Go_Foward(_fTimeDelta, m_pPlayerNavigation);
	}
	else if (pGameInstance->Get_DIKeyPress('A'))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_LWATTACKL);
		m_pPlayer->Play_Animation_All(false, "1hm_walkleftattackleft", m_pPlayer->Get_CurFrameIndex());
	
	}
	else if (pGameInstance->Get_DIKeyPress('D'))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_RWATTACKL);
		m_pPlayer->Play_Animation_All(false, "1hm_walkrightattackleft", m_pPlayer->Get_CurFrameIndex());
	
	}
	else if (pGameInstance->Get_DIKeyPress('S'))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_BWATTACKL);
		m_pPlayer->Play_Animation_All(false, "1hm_walkbwdattackleft", m_pPlayer->Get_CurFrameIndex());
	}

	Safe_Release(pGameInstance);

}

CStatePlayerOH_FwAttackL* CStatePlayerOH_FwAttackL::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_FwAttackL* pInstance = new CStatePlayerOH_FwAttackL();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_FwAttackL");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_FwAttackL::Free()
{
}
