#include "framework.h"
#include "StateFalmerUE_Detection.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"

CStateFalmerUE_Detection::CStateFalmerUE_Detection()
{
}

HRESULT CStateFalmerUE_Detection::Initialize(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerUE_Detection::Update(_float _fTimeDelta)
{
}

void CStateFalmerUE_Detection::Late_Update()
{
	if (dynamic_cast<CMonster*>(m_pMonster)->Get_IsAnimationFin())
	{
		m_pMonsterTransform->Set_Speed(1.5f);
		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Set_State(CFalmer_UnEquip::FALMERUE_RETURN);
		dynamic_cast<CFalmer_UnEquip*>(m_pMonster)->Play_Animation(true, "mtwalkforward");
	}
}

CStateFalmerUE_Detection* CStateFalmerUE_Detection::Create(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerUE_Detection* pInstance = new CStateFalmerUE_Detection();

	if (FAILED(pInstance->Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerUE_Detection");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerUE_Detection::Free()
{
}

