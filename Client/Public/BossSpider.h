#pragma once

#include "Monster.h"

BEGIN(Engine)

class CCollider;

END

BEGIN(Client)

class CBossSpider final : public CMonster
{
public:
	enum BOSSSPIDER_PARTS { PART_MOUTH, PART_LEFT, PART_RIGHT, PART_END };

	enum BOSSSPIDER_COLTYPE {
		BOSSSPIDER_COL_AABB,
		BOSSSPIDER_COL_DETECTION,
		BOSSSPIDER_COL_ATKROUND,
		BOSSSPIDER_COL_END
	};

	enum BOSSSPIDER_STATE {
		BOSSSPIDER_IDLE, 
		BOSSSPIDER_FALL, BOSSSPIDER_PLANDING, BOSSSPIDER_WARNING,
		BOSSSPIDER_CHASE,
		BOSSSPIDER_ONECHOP, BOSSSPIDER_DOUBLECHOP, BOSSSPIDER_BITE,
		// BOSSSPIDER_ICENIDLE, BOSSSPIDER_ICERANGEATK,
		BOSSSPIDER_STAGGER_ONECHOP, BOSSSPIDER_STAGGER_DOUBLECHOP, BOSSSPIDER_STAGGER_BITE,
		BOSSSPIDER_GROGGYINTRO, BOSSSPIDER_GROGGY, BOSSSPIDER_GROGGYOUT,
		BOSSSPIDER_DEAD,
		BOSSSPIDER_COMBATIDLE,
		BOSSSPIDER_END
	};

	typedef struct BossSpiderSpeedDesc
	{
		_float fChargeSpeed = 3.5f;
	}BOSSSPIDER_SPEEDDESC;



private:
	CBossSpider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBossSpider(const CBossSpider& rhs);
	virtual ~CBossSpider() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // 원본
	virtual HRESULT Initialize_Clone(void* pArg); // 사본
	virtual HRESULT Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg); // 사본
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Set_State(BOSSSPIDER_STATE _eState);

	BOSSSPIDER_SPEEDDESC   Get_BossSpiderSpeedDesc() { return m_tSpeedDesc; }

	/* 파츠 관련 */
	CGameObject* Get_Part(BOSSSPIDER_PARTS _ePart) { return m_vecMonsterPart[_ePart]; }

	/* 전투 시작 */
	void Set_Active() { m_bIsCombat = true; }

public:
	HRESULT Ready_Part();
	HRESULT Ready_Component(_uint _iLevel);
	HRESULT Ready_State();

private:
	vector<class CGameObject*>		m_vecMonsterPart;

	vector<CCollider*> m_pVecCollider;

	class CStateManager_BossSpider* m_pStateManager = nullptr;

	BOSSSPIDER_SPEEDDESC				m_tSpeedDesc;

	_bool							m_bIsCombat = false;
public:
	static CBossSpider* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual CGameObject* Clone(_uint _iLevel, const wstring& _strModelComTag, void* _pArg) override;
	virtual void Free() override;
};

END

