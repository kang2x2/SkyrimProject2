#include "framework.h"
#include "State_Player.h"

#include "GameInstance.h"

#include "Transform.h"
#include "Player.h"

CState_Player::CState_Player()
{
}

HRESULT CState_Player::Initialize(CGameObject* _pPlayer, class CTransform* _pPlayerTransform, class CNavigation* _pPlayerNavigation)
{
	if (_pPlayer == nullptr)
		return E_FAIL;

	m_pPlayer = dynamic_cast<CPlayer*>(_pPlayer);
	m_pPlayerTransform = _pPlayerTransform;
	m_pPlayerNavigation = _pPlayerNavigation;

	return S_OK;
}

void CState_Player::Update(_float _fTimeDelta)
{
}

void CState_Player::Late_Update()
{

}

void CState_Player::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyDown('F'))
	{
		m_pPlayer->Set_PlayerCam(m_pPlayer->Get_CurAnimationName().c_str(), m_pPlayer->Get_CurFrameIndex(), m_pPlayer->Get_CurAnimationIsLoop());

		/* 무기 */
		if (m_pPlayer->Get_PlayerEquipState() == CPlayer::EQUIP_ONEHAND)
			m_pPlayer->Set_SoketBone("WEAPON");
		else if (m_pPlayer->Get_PlayerEquipState() == CPlayer::EQUIP_UNEQUIP)
			m_pPlayer->Set_SoketBone("WeaponSword");

		/* 일반 상태 달리기가 이름이 다름. */
		if (m_pPlayer->Get_CamMode() == CPlayer::CAM_1ST &&
			m_pPlayer->Get_CurState() == CPlayer::UNEQUIP_RUN_F)
		{
			m_pPlayer->Play_Animation(true, "1hm_runforward", m_pPlayer->Get_CurFrameIndex());
		}
		else if (m_pPlayer->Get_CamMode() == CPlayer::CAM_3ST &&
			m_pPlayer->Get_CurState() == CPlayer::UNEQUIP_RUN_F)
		{
			m_pPlayer->Play_Animation(true, "mt_runforward", m_pPlayer->Get_CurFrameIndex());
		}
	}

	Safe_Release(pGameInstance);
}

void CState_Player::Player_SetLook(_float _fXDegree, _float _fYDegree)
{
	m_pPlayerTransform->Set_State(CTransform::STATE_LOOK, m_pPlayer->Get_PlayerCamLook());

	_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(_fXDegree));
	_matrix matRotX = XMMatrixRotationY(XMConvertToRadians(_fYDegree));

	_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));
	vPlayerLook = XMVector4Normalize(XMVector4Transform(vPlayerLook, matRotX));
	
	m_pPlayerTransform->SetLook(vPlayerLook);
}


void CState_Player::Free()
{
	__super::Free();
}
