#include "framework.h"
#include "StatePlayer_Idle.h"

#include "GameInstance.h"

#include "Player.h"


CStatePlayer_Idle::CStatePlayer_Idle()
{
}

HRESULT CStatePlayer_Idle::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform)
{
	__super::Initialize(_pPlayer, _pPlayerTransform);

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
	if (pGameInstance->Get_DIKeyPress('W'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_RUN_F);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_runforward");
	}

	if (pGameInstance->Get_DIKeyPress('S'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_RUN_B);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_runbackward");
	}

	if (pGameInstance->Get_DIKeyPress('A'))
	{
		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-90.f));
		_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

		m_pPlayerTransform->SetLook(vPlayerLook);

		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_RUN_L);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_runforward");
	}

	// 애니메이션 체인징 상태 끝났는지 확인하고 상태 전환하는 코드 추가.
	// 왼, 오 회전상태에서 멈추면 완전히 멈췄다가 정면을 봐야 한다.

	if (pGameInstance->Get_DIKeyPress('D'))
	{
		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(90.f));
		_vector vPlayerLook = XMVector4Normalize(XMVector4Transform(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotY));

		m_pPlayerTransform->SetLook(vPlayerLook);

		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_RUN_R);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "mt_runforward");
	}

	/* 공격 나중에 조건문 추가 */
	if (pGameInstance->Get_DIKeyDown('Z'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_LATTACK);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_attackleft");
	}

	if (pGameInstance->Get_DIKeyDown('X'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_RATTACK);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_attackright");
	}

	if (pGameInstance->Get_DIKeyDown('R'))
	{
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_PATTACK);
		dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(false, "1hm_attackpower");
	}

	Safe_Release(pGameInstance);

}

CStatePlayer_Idle* CStatePlayer_Idle::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform)
{
	CStatePlayer_Idle* pInstance = new CStatePlayer_Idle();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform)))
	{
		MSG_BOX("Fail Create : CStatePlayer_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayer_Idle::Free()
{
}
