#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "State_Monster.h"
#include "Skeever.h"

BEGIN(Client)

class CStateManager_Skeever final : public CBase
{
private:
	CStateManager_Skeever();
	virtual ~CStateManager_Skeever() = default;

public:
	CState_Monster* Get_State(CSkeever::SKEEVER_STATE _eState);

public:
	HRESULT Initialize(class CGameObject* _pMonster, class CTransform* _pPlayerTransform, class CNavigation* _pPlayerNavigation);
	HRESULT Set_State(CSkeever::SKEEVER_STATE _eState);

public:
	void Update(_float _fTimeDelta);
	void Late_Update();

private:
	CState_Monster* m_pCurState = nullptr;

	vector<class CState_Monster*> m_vecMonsterState; /* »óÅÂµé */

public:
	static CStateManager_Skeever* Create(class CGameObject* _pMonster, class CTransform* _pMonsterTransform, class CNavigation* _pMonsterNavigation);
	virtual void Free() override;
};

END