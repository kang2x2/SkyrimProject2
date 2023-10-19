#pragma once

#include "Client_Defines.h"
#include "State_Player.h"

BEGIN(Engine)

class CGameObject;
class CTransform;

END

BEGIN(Client)

class CStatePlayerOH_LAttack final : public CState_Player
{
private:
	CStatePlayerOH_LAttack();
	virtual ~CStatePlayerOH_LAttack() = default;

public:
	virtual		 HRESULT Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform);
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update();

public:
	static CStatePlayerOH_LAttack* Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform);
	virtual void Free() override;
};

END
