#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "State_FalmerUE.h"
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
	HRESULT Initialize(class CGameObject* _pMonster, class CTransform* _pPlayerTransform, class CNavigation* _pPlayerNavigation, vector<CCollider*> _pVecColCom);
	HRESULT Set_State(CFalmer_UnEquip::FALMERUE_STATE _eState);

public:
	void Update(_float _fTimeDelta);
	void Late_Update();

private:
	CState_FalmerUE* m_pCurState = nullptr;

	vector<class CState_FalmerUE*> m_vecMonsterState; /* »óÅÂµé */

public:
	static CStateManager_FalmerUE* Create(class CGameObject* _pMonster, class CTransform* _pMonsterTransform, class CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom);
	virtual void Free() override;
};

END