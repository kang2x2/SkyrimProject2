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

class CState_FalmerOH abstract : public CBase
{
protected:
	CState_FalmerOH();
	virtual ~CState_FalmerOH() = default;

public:
	virtual HRESULT Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom);

public:
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update();

protected:
	class CFalmer_OneHand* m_pMonster = nullptr;
	class CPlayer* m_pPlayer = nullptr;
	CTransform* m_pMonsterTransform = nullptr;
	CNavigation* m_pMonsterNavigation = nullptr;

	vector<CCollider*>  m_pVecCollider;

	_bool			m_isReadyAtk = true;

public:
	virtual void Free() override;

};

END

