#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CState_Player abstract : public CBase
{
protected:
	CState_Player();
	virtual ~CState_Player() = default;

public:
	virtual HRESULT Initialize(class CGameObject* _pPlayer, class CTransform* _pPlayerTransform);

public:
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update();

	virtual void Key_Input(_float _fTimeDelta);

protected:
	class CGameObject* m_pPlayer = nullptr;
	class CTransform*  m_pPlayerTransform = nullptr;

public:
	virtual void Free() override;
};

END

