#pragma once

#include "State_BossSpider.h"

BEGIN(Engine)

class CGameObject;
class CTransform;
class CNavigation;

END

BEGIN(Client)

class CStateManager_BossSpider;
class CStateBossSpider_BerserkChop final : public CState_BossSpider
{
private:
	CStateBossSpider_BerserkChop();
	virtual ~CStateBossSpider_BerserkChop() = default;

public:
	virtual		 HRESULT Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom);
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update(_float _fTimeDelta);

public:
	static CStateBossSpider_BerserkChop* Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom);
	virtual void Free() override;
};

END