#pragma once

#include "Client_Defines.h"
#include "State_Player.h"

BEGIN(Engine)

class CGameObject;
class CTransform;

END

BEGIN(Client)

class CStatePlayerOH_RunPAttack final : public CState_Player
{
private:
	CStatePlayerOH_RunPAttack();
	virtual ~CStatePlayerOH_RunPAttack() = default;

public:
	virtual		 HRESULT Initialize(CGameObject* _pPlayer, CTransform* _pPlayerTransform);
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update();

public:
	static CStatePlayerOH_RunPAttack* Create(CGameObject* _pPlayer, CTransform* _pPlayerTransform);
	virtual void Free() override;
};

END
