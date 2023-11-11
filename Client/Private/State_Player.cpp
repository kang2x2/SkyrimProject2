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

void CState_Player::Key_Input()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyDown('F'))
		m_pPlayer->Set_PlayerCam();

	Safe_Release(pGameInstance);
}

void CState_Player::Player_SetLook(_float _fDegree)
{
	_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(_fDegree));
	_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

	m_pPlayerTransform->SetLook(vPlayerLook);
}


void CState_Player::Free()
{
	__super::Free();
}
