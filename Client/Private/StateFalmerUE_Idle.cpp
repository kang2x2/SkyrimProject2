#include "framework.h"
#include "StateFalmerUE_Idle.h"

#include "GameInstance.h"

#include "Skeever.h"

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
	Key_Input(_fTimeDelta);
}

void CStateFalmerUE_Idle::Late_Update()
{
}

void CStateFalmerUE_Idle::Key_Input(_float _fTimeDelta)
{
}

CStateFalmerUE_Idle* CStateFalmerUE_Idle::Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation)
{
	return nullptr;
}

void CStateFalmerUE_Idle::Free()
{
}

