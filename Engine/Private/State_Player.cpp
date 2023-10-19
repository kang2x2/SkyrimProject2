#include "State_Player.h"

CState_Player::CState_Player()
{
}

HRESULT CState_Player::Initialize(CGameObject* _pPlayer, class CTransform* _pPlayerTransform)
{
	if (_pPlayer == nullptr)
		return E_FAIL;

	m_pPlayer = _pPlayer;
	m_pPlayerTransform = _pPlayerTransform;

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
}


void CState_Player::Free()
{
	__super::Free();
}
