#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CState_Monster abstract : public CBase
{
protected:
	CState_Monster();
	virtual ~CState_Monster() = default;

public:
	virtual HRESULT Initialize(class CGameObject* _pMonster, class CTransform* _pPlayerTransform, class CNavigation* _pPlayerNavigation);

public:
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update();

	virtual void Key_Input(_float _fTimeDelta);

protected:
	class CGameObject* m_pMonster = nullptr;
	class CTransform* m_pMonsterTransform = nullptr;
	class CNavigation* m_pMonsterNavigation = nullptr;

public:
	virtual void Free() override;
};

END

