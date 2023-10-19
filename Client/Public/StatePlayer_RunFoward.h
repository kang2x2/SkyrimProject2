#pragma once

#include "Client_Defines.h"
#include "State_Player.h"

BEGIN(Engine)

class CGameObject;
class CTransform;

END

BEGIN(Client)

class CStatePlayer_RunFoward final : public CState_Player
{
private:
	CStatePlayer_RunFoward();
	virtual ~CStatePlayer_RunFoward() = default;

public:
	virtual		 HRESULT Initialize(CGameObject * _pPlayer, CTransform * _pPlayerTransform);
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update();
	virtual void Key_Input(_float _fTimeDelta);

public:
	static CStatePlayer_RunFoward* Create(CGameObject * _pPlayer, CTransform * _pPlayerTransform);
	virtual void Free() override;
};

END
