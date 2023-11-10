#include "framework.h"
#include "StatePlayerOH_LAttack.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_LAttack::CStatePlayerOH_LAttack()
{
}

HRESULT CStatePlayerOH_LAttack::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_LAttack::Update(_float _fTimeDelta)
{
	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CamMode() == CPlayer::CAM_1ST)
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

	dynamic_cast<CPlayer*>(m_pPlayer)->CheckHit_Onehand(13, 15);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_CurFrameIndex() >= 25 &&
		!dynamic_cast<CPlayer*>(m_pPlayer)->Get_CurAnimationName("1hm_attackright") &&
		pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RATTACK);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_attackright");
	}

	Safe_Release(pGameInstance);
}

void CStatePlayerOH_LAttack::Late_Update()
{
	if (dynamic_cast<CPlayer*>(m_pPlayer)->Get_IsAnimationFin())
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_IDLE);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_idle");
	}
}

CStatePlayerOH_LAttack* CStatePlayerOH_LAttack::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_LAttack* pInstance = new CStatePlayerOH_LAttack();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_LAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_LAttack::Free()
{
}
