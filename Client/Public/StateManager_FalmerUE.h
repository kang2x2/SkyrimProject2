#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "State_Monster.h"
#include "Falmer_UnEquip.h"

BEGIN(Client)

class CStateManager_FalmerUE final : public CBase
{
private:
	CStateManager_FalmerUE();
	virtual ~CStateManager_FalmerUE() = default;

public:
	CState_Monster* Get_State(CFalmer_UnEquip::FALMERUE_STATE _eState);

public:
	HRESULT Initialize(class CGameObject* _pMonster, class CTransform* _pPlayerTransform, class CNavigation* _pPlayerNavigation);
	HRESULT Set_State(CFalmer_UnEquip::FALMERUE_STATE _eState);

public:
	void Update(_float _fTimeDelta);
	void Late_Update();

private:
	CState_Monster* m_pCurState = nullptr;

	vector<class CState_Monster*> m_vecMonsterState; /* »óÅÂµé */

public:
	static CStateManager_FalmerUE* Create(class CGameObject* _pMonster, class CTransform* _pMonsterTransform, class CNavigation* _pMonsterNavigation);
	virtual void Free() override;
};

END