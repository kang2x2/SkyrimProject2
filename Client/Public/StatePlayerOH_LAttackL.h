#pragma once

#include "Client_Defines.h"
#include "State_Player.h"

BEGIN(Engine)

class CGameObject;
class CTransform;
class CNavigation;

END

BEGIN(Client)

class CStatePlayerOH_LAttackL final : public CState_Player
{
private:
	CStatePlayerOH_LAttackL();
	virtual ~CStatePlayerOH_LAttackL() = default;

public:
	virtual		 HRESULT Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation);
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update();

	virtual void Key_Input(_float _fTimeDelta);

public:
	static CStatePlayerOH_LAttackL* Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation);
	virtual void Free() override;
};

END
