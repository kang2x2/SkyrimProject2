#include "framework.h"
#include "StatePlayerOH_RAttackR.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_RAttackR::CStatePlayerOH_RAttackR()
{
}

HRESULT CStatePlayerOH_RAttackR::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_RAttackR::Update(_float _fTimeDelta)
{
	Key_Input(_fTimeDelta);

	m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CurFrameIndex() >= 25 &&
		dynamic_cast<CPlayer*>(m_pPlayer)->Get_CurAnimationName("1hm_walkrtattackright") &&
		pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_LATTACKR);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_walkrightattackleft");
	}

	Safe_Release(pGameInstance);
}

void CStatePlayerOH_RAttackR::Late_Update()
{
	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_IsAnimationFin())
	{
		m_pPlayerTransform->Set_Speed(dynamic_cast<CPlayer*>(m_pPlayer)->GetRunSpeed());

		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_IDLE);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_idle");
	}
}

void CStatePlayerOH_RAttackR::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyPress('A'))
	{
		m_pPlayerTransform->Go_Left(_fTimeDelta, m_pPlayerNavigation);
	}

	else if (pGameInstance->Get_DIKeyPress('D'))
	{
		m_pPlayerTransform->Go_Right(_fTimeDelta, m_pPlayerNavigation);
	}

	Safe_Release(pGameInstance);

}

CStatePlayerOH_RAttackR* CStatePlayerOH_RAttackR::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_RAttackR* pInstance = new CStatePlayerOH_RAttackR();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_RAttackR");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_RAttackR::Free()
{
}
