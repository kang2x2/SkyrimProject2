#include "framework.h"
#include "StatePlayerOH_Block.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayerOH_Block::CStatePlayerOH_Block()
{
}

HRESULT CStatePlayerOH_Block::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	__super::Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation);

	return S_OK;
}

void CStatePlayerOH_Block::Update(_float _fTimeDelta)
{
	Key_Input(_fTimeDelta);
}

void CStatePlayerOH_Block::Late_Update()
{
	if (m_pPlayer->Get_IsAnimationFin() &&
		m_pPlayer->Get_CurAnimationName("1hm_equip"))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_IDLE);
		m_pPlayer->Play_Animation(true, "1hm_idle");
	}
}

void CStatePlayerOH_Block::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyUp(VK_RBUTTON))
	{
		m_pPlayer->Set_State(CPlayer::ONEHAND_IDLE);
		m_pPlayer->Play_Animation(true, "1hm_idle");
	}

	Safe_Release(pGameInstance);

	__super::Key_Input();
}

CStatePlayerOH_Block* CStatePlayerOH_Block::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	CStatePlayerOH_Block* pInstance = new CStatePlayerOH_Block();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform, _pPlayerNavigation)))
	{
		MSG_BOX("Fail Create : CStatePlayerOH_Block");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayerOH_Block::Free()
{
}
