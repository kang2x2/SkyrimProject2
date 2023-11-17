#pragma once

#include "Client_Defines.h"
#include "State_FalmerUE.h"

BEGIN(Engine)

class CGameObject;
class CTransform;
class CNavigation;

END

BEGIN(Client)

class CStateManager_FalmerUE;
class CStateFalmerUE_StaggerL final : public CState_FalmerUE
{
private:
	CStateFalmerUE_StaggerL();
	virtual ~CStateFalmerUE_StaggerL() = default;

public:
	virtual		 HRESULT Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom);
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update();

public:
	static CStateFalmerUE_StaggerL* Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom);
	virtual void Free() override;
};

END

