#pragma once

#include "Client_Defines.h"
#include "State_FalmerOH.h"

BEGIN(Engine)

class CGameObject;
class CTransform;
class CNavigation;

END

BEGIN(Client)

class CStateManager_FalmerOH;
class CStateFalmerOH_UnEquip final : public CState_FalmerOH
{
private:
	CStateFalmerOH_UnEquip();
	virtual ~CStateFalmerOH_UnEquip() = default;

public:
	virtual		 HRESULT Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom);
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update(_float _fTimeDelta);

public:
	static CStateFalmerOH_UnEquip* Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom);
	virtual void Free() override;
};

END

