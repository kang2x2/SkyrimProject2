#include "framework.h"
#include "StatePlayerOH_BwAttackL.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_BwAttackL::CStatePlayerOH_BwAttackL()
{
}

HRESULT CStatePlayerOH_BwAttackL::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_BwAttackL::Update(_float _fTimeDelta)
{
	if (m_pPlayer->Get_CamMode() == CPlayer::CAM_3ST)
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

	if (m_pPlayer->Get_CurFrameIndex() >= 14 && m_pPlayer->Get_CurFrameIndex() <= 18 &&
		!strcmp(m_pPlayer->Get_CurAnimationName().c_str(), "1hm_walkbwdattackleft"))
	{
		m_pPlayer->Set_IsAttack(true);
	}
	else
		m_pPlayer->Set_IsAttack(false);


	Key_Input(_fTimeDelta);

	__super::Key_Input(_fTimeDelta);
}

void CStatePlayerOH_BwAttackL::Late_Update(_float _fTimeDelta)
{
	if (m_pPlayer->Get_IsAnimationFin() && 
		!strcmp(m_pPlayer->Get_CurAnimationName().c_str(), "1hm_walkbwdattackleft"))
	{
		m_pPlayer->Set_IsAttack(false);

		m_pPlayerTransform->Set_Speed(m_pPlayer->GetRunSpeed());

		m_pPlayer->Set_State(CPlayer::ONEHAND_IDLE);
		m_pPlayer->Play_Animation_All(true, "1hm_idle");
	}
}

void CStatePlayerOH_BwAttackL::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_pPlayer->Get_CurFrameIndex() >= 20 &&
		!strcmp(m_pPlayer->Get_CurAnimationName().c_str(), "1hm_walkbwdattackleft") &&
		pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_BWATTACKR);
		m_pPlayer->Play_Animation_All(false, "1hm_walkbwdattackright");
	}

	else if (m_pPlayer->Get_CurFrameIndex() >= 20 && m_pPlayer->Get_PlayerSp() > 0.f &&
		!strcmp(m_pPlayer->Get_CurAnimationName().c_str(), "1hm_walkbwdattackleft") &&
		pGameInstance->Get_DIKeyDown('V'))
	{
		m_pPlayer->Set_PlayerSp(-20.f);

		m_pPlayer->Set_State(CPlayer::ONEHAND_PATTACK);
		m_pPlayer->Play_Animation_All(false, "1hm_attackpower");
	}

	if (pGameInstance->Get_DIKeyPress('S'))
	{
		m_pPlayerTransform->Go_Backward(_fTimeDelta, m_pPlayerNavigation);
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
	else if (pGameInstance->Get_DIKeyPress('W'))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_FWATTACKL);
		m_pPlayer->Play_Animation_All(false, "1hm_walkfwdattackleft", m_pPlayer->Get_CurFrameIndex());
	}

	Safe_Release(pGameInstance);

}

CStatePlayerOH_BwAttackL* CStatePlayerOH_BwAttackL::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_BwAttackL* pInstance = new CStatePlayerOH_BwAttackL();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_BwAttackL");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_BwAttackL::Free()
{
}
