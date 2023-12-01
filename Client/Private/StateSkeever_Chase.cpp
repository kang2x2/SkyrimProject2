#include "framework.h"
#include "StateSkeever_Chase.h"

#include "GameInstance.h"

#include "Skeever.h"
#include "Player.h"

CStateSkeever_Chase::CStateSkeever_Chase()
{
}

HRESULT CStateSkeever_Chase::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateSkeever_Chase::Update(_float _fTimeDelta)
{
	m_pMonsterTransform->Go_Foward(_fTimeDelta, m_pMonsterNavigation);
	m_pMonsterTransform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* �߰� ������ ����� �� */
	if (!pGameInstance->Collision_Stay(m_pVecCollider[CSkeever::SKEEVER_COL_MISSDETECTION], m_pPlayerBodyCollider))
	{
		m_pMonster->Set_State(CSkeever::SKEEVER_DETECTION);
		m_pMonster->Play_Animation(false, "idlecombat1");
	}

	/* RunPowerAtk ������ ������ �� */
	if (pGameInstance->Collision_Enter(m_pVecCollider[CSkeever::SKEEVER_COL_ATKROUND], m_pPlayerBodyCollider))
	{
		m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

		m_pMonster->Set_State(CSkeever::SKEEVER_CHARGE);
		m_pMonster->Play_Animation(false, "attackpowerforward");
	}

	Safe_Release(pGameInstance);
}

void CStateSkeever_Chase::Late_Update()
{

}

CStateSkeever_Chase* CStateSkeever_Chase::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateSkeever_Chase* pInstance = new CStateSkeever_Chase();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateSkeever_Chase");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateSkeever_Chase::Free()
{
}

