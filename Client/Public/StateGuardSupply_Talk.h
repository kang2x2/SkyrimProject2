#pragma once

#include "Client_Defines.h"
#include "State_GuardSupply.h"

BEGIN(Engine)

class CGameObject;
class CTransform;
class CNavigation;

END

BEGIN(Client)

class CStateManager_GuardSupply;
class CStateGuardSupply_Talk final : public CState_GuardSupply
{
private:
	CStateGuardSupply_Talk();
	virtual ~CStateGuardSupply_Talk() = default;

public:
	virtual		 HRESULT Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom);
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update(_float _fTimeDelta);

public:
	static CStateGuardSupply_Talk* Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom);
	virtual void Free() override;
};

END

