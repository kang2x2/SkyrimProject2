#include "framework.h"
#include "StateSkeever_Idle.h"

#include "GameInstance.h"

#include "Skeever.h"

CStateSkeever_Idle::CStateSkeever_Idle()
{
}

HRESULT CStateSkeever_Idle::Initialize(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation)
{
	__super::Initialize(_pMonster, _pMonsterTransform, _pMonsterNavigation);

	return S_OK;
}

void CStateSkeever_Idle::Update(_float _fTimeDelta)
{
}

void CStateSkeever_Idle::Late_Update()
{
}

CStateSkeever_Idle* CStateSkeever_Idle::Create(CGameObject* _pMonster, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation)
{
	return nullptr;
}

void CStateSkeever_Idle::Free()
{
}
