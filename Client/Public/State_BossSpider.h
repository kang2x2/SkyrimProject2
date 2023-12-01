#pragma once

#include "State_Monster.h"

BEGIN(Engine)

class CGameObject;
class CTransform;
class CNavigation;
class CCollider;

END

BEGIN(Client)

class CState_BossSpider abstract : public CState_Monster
{
public:
	CState_BossSpider();
	virtual ~CState_BossSpider() = default;

public:
	virtual HRESULT Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom);

public:
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update();

protected:
	class CBossSpider* m_pMonster = nullptr;
	CCollider* m_pMouthCollider = nullptr;
	CCollider* m_pLeftCollider = nullptr;
	CCollider* m_pRightCollider = nullptr;

public:
	virtual void Free() override;
};

END

