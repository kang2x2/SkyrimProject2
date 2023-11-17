#include "framework.h"
#include "StateSkeever_Stagger.h"

#include "GameInstance.h"

#include "Skeever.h"
#include "Player.h"

CStateSkeever_Stagger::CStateSkeever_Stagger()
{
}

HRESULT CStateSkeever_Stagger::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateSkeever_Stagger::Update(_float _fTimeDelta)
{
}

void CStateSkeever_Stagger::Late_Update()
{
	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "recoil"))
	{
		m_isReadyAtk = true;

		m_pMonster->Set_State(CSkeever::SKEEVER_CHASE);
		m_pMonster->Play_Animation(true, "runforward");
	}

}

CStateSkeever_Stagger* CStateSkeever_Stagger::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateSkeever_Stagger* pInstance = new CStateSkeever_Stagger();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateSkeever_Stagger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateSkeever_Stagger::Free()
{
}
