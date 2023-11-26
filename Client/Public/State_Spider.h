#pragma once

#include "State_Monster.h"

BEGIN(Engine)

class CGameObject;
class CTransform;
class CNavigation;
class CCollider;

END

BEGIN(Client)

class CState_Spider abstract : public CState_Monster
{
protected:
	CState_Spider();
	virtual ~CState_Spider() = default;

public:
	virtual HRESULT Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom);

public:
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update();

protected:
	class CSpider* m_pMonster = nullptr;
	CCollider* m_pWeaponCollider = nullptr;

public:
	virtual void Free() override;

};

END

