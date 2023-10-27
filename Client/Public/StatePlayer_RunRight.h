#pragma once

#include "Client_Defines.h"
#include "State_Player.h"

BEGIN(Engine)

class CGameObject;
class CTransform;
class CNavigation;

END

BEGIN(Client)

class CStatePlayer_RunRight final : public CState_Player
{
private:
	CStatePlayer_RunRight();
	virtual ~CStatePlayer_RunRight() = default;

public:
	virtual		 HRESULT Initialize(CGameObject * _pPlayer, CTransform * _pPlayerTransform, CNavigation* _pPlayerNavigation);
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update();
	virtual void Key_Input(_float _fTimeDelta);

public:
	static CStatePlayer_RunRight* Create(CGameObject * _pPlayer, CTransform * _pPlayerTransform, CNavigation* _pPlayerNavigation);
	virtual void Free() override;
};

END
