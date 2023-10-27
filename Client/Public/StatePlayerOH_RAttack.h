#pragma once

#include "Client_Defines.h"
#include "State_Player.h"

BEGIN(Engine)

class CGameObject;
class CTransform;
class CNavigation;

END

BEGIN(Client)

class CStatePlayerOH_RAttack final : public CState_Player
{
private:
	CStatePlayerOH_RAttack();
	virtual ~CStatePlayerOH_RAttack() = default;

public:
	virtual		 HRESULT Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation);
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update();

public:
	static CStatePlayerOH_RAttack* Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation);
	virtual void Free() override;
};

END
