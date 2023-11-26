#include "framework.h"
#include "StateSpider_Chase.h"

#include "GameInstance.h"

#include "Spider.h"
#include "Player.h"

CStateSpider_Chase::CStateSpider_Chase()
{
}

HRESULT CStateSpider_Chase::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateSpider_Chase::Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);
}

void CStateSpider_Chase::Late_Update()
{

}

CStateSpider_Chase* CStateSpider_Chase::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateSpider_Chase* pInstance = new CStateSpider_Chase();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateSpider_Chase");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateSpider_Chase::Free()
{
}

