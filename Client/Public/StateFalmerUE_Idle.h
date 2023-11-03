#pragma once

#include "Client_Defines.h"
#include "State_Monster.h"

BEGIN(Engine)

class CGameObject;
class CTransform;
class CNavigation;

END

BEGIN(Client)

class CStateManager_FalmerUE;
class CStateFalmerUE_Idle final : public CState_Monster
{
private:
	CStateFalmerUE_Idle();
	virtual ~CStateFalmerUE_Idle() = default;

public:
	virtual		 HRESULT Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation);
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update();
	virtual void Key_Input(_float _fTimeDelta);

public:
	static CStateFalmerUE_Idle* Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation);
	virtual void Free() override;
};

END

