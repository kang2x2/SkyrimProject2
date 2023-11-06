#include "framework.h"
#include "StatePlayerOH_Idle.h"

#include "GameInstance.h"

#include "Player.h"


CStatePlayerOH_Idle::CStatePlayerOH_Idle()
{
}

HRESULT CStatePlayerOH_Idle::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_Idle::Update(_float _fTimeDelta)
{
	// m_pPlayerTransform->Fix_Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0.f));

	Key_Input(_fTimeDelta);
}

void CStatePlayerOH_Idle::Late_Update()
{

}

void CStatePlayerOH_Idle::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* 이동 */
	if (pGameInstance->Get_DIKeyDown('W'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RUN_F);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_runforward");
	}

	if (pGameInstance->Get_DIKeyDown('S'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RUN_B);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_runbackward");
	}

	if (pGameInstance->Get_DIKeyDown('A'))
	{
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-90.f));
		_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

		m_pPlayerTransform->SetLook(vPlayerLook);

		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RUN_L);

		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_runforward");
	}

	// 애니메이션 체인징 상태 끝났는지 확인하고 상태 전환하는 코드 추가.
	// 왼, 오 회전상태에서 멈추면 완전히 멈췄다가 정면을 봐야 한다.

	if (pGameInstance->Get_DIKeyDown('D'))
	{
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(90.f));
		_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

		m_pPlayerTransform->SetLook(vPlayerLook);

		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_RUN_R);

		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "1hm_runforward");
	}

	if (pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_LATTACK);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_attackleft");
	}

	if (pGameInstance->Get_DIKeyDown('R'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_PlayerEquipState(CPlayer::EQUIP_NONEQUIP);

		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_UNEQUIP);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_unequip");
	}

	//dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_PATTACK);
	//dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_equip");
	
	//1hm_attackpower

	Safe_Release(pGameInstance);

}

CStatePlayerOH_Idle* CStatePlayerOH_Idle::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_Idle* pInstance = new CStatePlayerOH_Idle();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_Idle::Free()
{
}
