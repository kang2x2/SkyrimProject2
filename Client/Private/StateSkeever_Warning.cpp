#include "framework.h"
#include "StateSkeever_Warning.h"

#include "GameInstance.h"

#include "Skeever.h"
#include "Player.h"

CStateSkeever_Warning::CStateSkeever_Warning()
{
}

HRESULT CStateSkeever_Warning::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateSkeever_Warning::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);

	m_pMonsterTransform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
}

void CStateSkeever_Warning::Late_Update()
{
	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "aggrowarning1"))
	{
		m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

		m_pMonster->Set_State(CSkeever::SKEEVER_CHASE);
		m_pMonster->Play_Animation(true, "runforward");
	}

}

CStateSkeever_Warning* CStateSkeever_Warning::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateSkeever_Warning* pInstance = new CStateSkeever_Warning();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateSkeever_Warning");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateSkeever_Warning::Free()
{
}
