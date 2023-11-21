#include "framework.h"
#include "StatePlayerOH_RunPAttack.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_RunPAttack::CStatePlayerOH_RunPAttack()
{
}

HRESULT CStatePlayerOH_RunPAttack::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_RunPAttack::Update(_float _fTimeDelta)
{
	if (m_pPlayer->Get_CamMode() == CPlayer::CAM_3ST)
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

	m_pPlayerTransform->Go_Foward(_fTimeDelta);

	__super::Key_Input(_fTimeDelta);
}

void CStatePlayerOH_RunPAttack::Late_Update()
{
	if (m_pPlayer->Get_IsAnimationFin())
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_IDLE);
		m_pPlayer->Play_Animation_All(true, "1hm_idle");
	}
}

CStatePlayerOH_RunPAttack* CStatePlayerOH_RunPAttack::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_RunPAttack* pInstance = new CStatePlayerOH_RunPAttack();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_RunPAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_RunPAttack::Free()
{
}
