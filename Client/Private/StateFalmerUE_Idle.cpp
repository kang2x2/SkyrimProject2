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
	1. �ȸ� Ŭ������ bool �Լ��� �����ϴ� �Լ� ����.(detection)
	2. true�� �޸���� ��ȯ
	3. false�� ��� ȸ��.
	�ƴϸ� �׳� state�� Collider�� ��� �ְ� �ص� ������ ��??? 
	�̰� ���ڴ�.
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

