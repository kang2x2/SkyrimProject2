#include "framework.h"
#include "StatePlayer_Idle.h"

#include "GameInstance.h"

#include "Player.h"


CStatePlayer_Idle::CStatePlayer_Idle()
{
}

HRESULT CStatePlayer_Idle::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayer_Idle::Update(_float _fTimeDelta)
{
	// m_pPlayerTransform->Fix_Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0.f));

	Key_Input(_fTimeDelta);
}

void CStatePlayer_Idle::Late_Update()
{

}

void CStatePlayer_Idle::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* 이동 */
	if (pGameInstance->Get_DIKeyDown('W'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ENEQUIP_RUN_F);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_runforward");
	}

	if (pGameInstance->Get_DIKeyDown('S'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ENEQUIP_RUN_B);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_runbackward");
	}

	if (pGameInstance->Get_DIKeyDown('A'))
	{
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-90.f));
		_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

		m_pPlayerTransform->SetLook(vPlayerLook);

		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ENEQUIP_RUN_L);
		
		// 분명히 필요하긴 할거야 이 함수가.
		//if(dynamic_cast<CPlayer*>(m_pPlayer)->Get_CurAnimationName("1hm_runforward"))
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_runforward");
	}

	if (pGameInstance->Get_DIKeyDown('D'))
	{
		m_pPlayerTransform->SetLook(dynamic_cast<CPlayer*>(m_pPlayer)->Get_PlayerCamLook());

		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(90.f));
		_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

		m_pPlayerTransform->SetLook(vPlayerLook);

		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ENEQUIP_RUN_R);
		
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_runforward");
	}

	if (pGameInstance->Get_DIKeyDown('R'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_PlayerEquipState(CPlayer::EQUIP_ONEHAND);
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(CPlayer::ONEHAND_EQUIP);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_equip");
	}

	Safe_Release(pGameInstance);

}

CStatePlayer_Idle* CStatePlayer_Idle::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayer_Idle* pInstance = new CStatePlayer_Idle();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayer_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayer_Idle::Free()
{
}
