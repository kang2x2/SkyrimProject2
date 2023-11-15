#include "framework.h"
#include "StatePlayerOH_LAttackL.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_LAttackL::CStatePlayerOH_LAttackL()
{
}

HRESULT CStatePlayerOH_LAttackL::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_LAttackL::Update(_float _fTimeDelta)
{
	if (m_pPlayer->Get_CamMode() == CPlayer::CAM_3ST)
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

	Key_Input(_fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_pPlayer->Get_CurFrameIndex() >= 25 &&
		m_pPlayer->Get_CurAnimationName("1hm_walkleftattackleft") &&
		pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_RATTACKL);
		m_pPlayer->Play_Animation(false, "1hm_walkleftattackright");
	}

	Safe_Release(pGameInstance);

	__super::Key_Input(_fTimeDelta);
}

void CStatePlayerOH_LAttackL::Late_Update()
{
	if (m_pPlayer->Get_IsAnimationFin())
	{
		m_pPlayerTransform->Set_Speed(m_pPlayer->GetRunSpeed());

		m_pPlayer->Set_State(CPlayer::ONEHAND_IDLE);
		m_pPlayer->Play_Animation(true, "1hm_idle");
	}
}

void CStatePlayerOH_LAttackL::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyPress('A'))
	{
		m_pPlayerTransform->Go_Left(_fTimeDelta, m_pPlayerNavigation);
	}
	if (pGameInstance->Get_DIKeyPress('D'))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_LATTACKR);
		m_pPlayer->Play_Animation(false, "1hm_walkrightattackleft", m_pPlayer->Get_CurFrameIndex());
	}
	if (pGameInstance->Get_DIKeyPress('W'))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_LATTACKF);
		m_pPlayer->Play_Animation(false, "1hm_walkfwdattackleft", m_pPlayer->Get_CurFrameIndex());
	}
	if (pGameInstance->Get_DIKeyPress('S'))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_LATTACKB);
		m_pPlayer->Play_Animation(false, "1hm_walkbwdattackleft", m_pPlayer->Get_CurFrameIndex());
	}

	Safe_Release(pGameInstance);
}

CStatePlayerOH_LAttackL* CStatePlayerOH_LAttackL::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_LAttackL* pInstance = new CStatePlayerOH_LAttackL();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_LAttackL");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_LAttackL::Free()
{
}
