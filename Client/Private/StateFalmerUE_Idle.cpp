#include "framework.h"
#include "StateFalmerUE_Idle.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"

CStateFalmerUE_Idle::CStateFalmerUE_Idle()
{
}

HRESULT CStateFalmerUE_Idle::Initialize(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation)
{
	__super::Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation);

	return S_OK;
}

void CStateFalmerUE_Idle::Update(_float _fTimeDelta)
{
	/*  
	1. 팔머 클래스에 bool 함수로 추적하는 함수 생성.(detection)
	2. true면 달리기로 전환
	3. false면 계속 회전.
	아니면 그냥 state가 Collider을 들고 있게 해도 괜찮을 듯??? 
	이게 낫겠다.
	*/
}

void CStateFalmerUE_Idle::Late_Update()
{

}

CStateFalmerUE_Idle* CStateFalmerUE_Idle::Create(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation)
{
	CStateFalmerUE_Idle* pInstance = new CStateFalmerUE_Idle();

	if (FAILED(pInstance->Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation)))
	{
		MSG_BOX("Fail Create : CStateFalmerUE_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerUE_Idle::Free()
{
}

