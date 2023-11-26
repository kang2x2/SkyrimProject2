#pragma once

#include "Client_Defines.h"
#include "State_Spider.h"

BEGIN(Engine)

class CGameObject;
class CTransform;
class CNavigation;

END

BEGIN(Client)

class CStateManager_Spider;
class CStateSpider_GetUp final : public CState_Spider
{
private:
	CStateSpider_GetUp();
	virtual ~CStateSpider_GetUp() = default;

public:
	virtual		 HRESULT Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom);
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update();

public:
	static CStateSpider_GetUp* Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom);
	virtual void Free() override;
};

END

