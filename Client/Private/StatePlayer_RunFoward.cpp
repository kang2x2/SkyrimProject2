#include "framework.h"
#include "StatePlayer_RunFoward.h"

#include "GameInstance.h"

#include "Player.h"

CStatePlayer_RunFoward::CStatePlayer_RunFoward()
{
}

HRESULT CStatePlayer_RunFoward::Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform)
{
	__super::Initialize(_pPlayer, _pPlayerTransform);

	return S_OK;
}

void CStatePlayer_RunFoward::Update(_float _fTimeDelta)
{
	m_pPlayerTransform->Go_Foward(_fTimeDelta);
	dynamic_cast<CPlayer*>(m_pPlayer)->Play_Animation(true, "RunFoward");

	Key_Input(_fTimeDelta);
}

void CStatePlayer_RunFoward::Late_Update()
{
}

void CStatePlayer_RunFoward::Key_Input(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	if (pGameInstance->Get_DIKeyUp('W'))
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_IDLE);

	if (pGameInstance->Get_DIKeyPress('A'))
		m_pPlayerTransform->Fix_Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-45.0f));

	if (pGameInstance->Get_DIKeyPress('D'))
		m_pPlayerTransform->Fix_Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.0f));

	if (pGameInstance->Get_DIKeyDown('R'))
		dynamic_cast<CPlayer*>(m_pPlayer)->Set_State(ONEHAND_RUNPOWERATTACK);



	Safe_Release(pGameInstance);

}

CStatePlayer_RunFoward* CStatePlayer_RunFoward::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform)
{
	CStatePlayer_RunFoward* pInstance = new CStatePlayer_RunFoward();

	if (FAILED(pInstance->Initialize(_pPlayer, _pPlayerTransform)))
	{
		MSG_BOX("Fail Create : CStatePlayer_RunFoward");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatePlayer_RunFoward::Free()
{
}
