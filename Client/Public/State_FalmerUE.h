#pragma once

#include "Client_Defines.h"
#include "State_Monster.h"

#include "Player.h"

BEGIN(Engine)

class CCollider;

END

BEGIN(Client)

class CState_FalmerUE abstract : public CState_Monster
{
protected:
	CState_FalmerUE();
	virtual ~CState_FalmerUE() = default;

public:
	virtual HRESULT Initialize(CGameObject* _pMonster, CTransform* _pPlayerTransform, CNavigation* _pPlayerNavigation, vector<CCollider*> _pVecColCom);

public:
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update();

protected:
	vector<CCollider*>  m_pVecCollider;

public:
	virtual void Free() override;

};

END

