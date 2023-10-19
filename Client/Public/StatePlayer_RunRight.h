#pragma once

#include "Client_Defines.h"
#include "State_Player.h"

BEGIN(Engine)

class CGameObject;
class CTransform;

END

BEGIN(Client)

class CStatePlayer_RunRight final : public CState_Player
{
private:
	CStatePlayer_RunRight();
	virtual ~CStatePlayer_RunRight() = default;

public:
	virtual		 HRESULT Initialize(CGameObject * _pPlayer, CTransform * _pPlayerTransform);
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update();

public:
	static CStatePlayer_RunRight* Create(CGameObject * _pPlayer, CTransform * _pPlayerTransform);
	virtual void Free() override;
};

END
