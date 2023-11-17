#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CGameObject;
class CTransform;
class CNavigation;
class CCollider;

END

BEGIN(Client)

class CState_FalmerUE abstract : public CBase
{
protected:
	CState_FalmerUE();
	virtual ~CState_FalmerUE() = default;

public:
	virtual HRESULT Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom);

public:
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update();

protected:
	class CFalmer_UnEquip* m_pMonster = nullptr;
	class CPlayer* m_pPlayer = nullptr;
	CTransform* m_pMonsterTransform = nullptr;
	CNavigation* m_pMonsterNavigation = nullptr;

	vector<CCollider*>  m_pVecCollider;

	_bool			m_isReadyAtk = true;

public:
	virtual void Free() override;

};

END

