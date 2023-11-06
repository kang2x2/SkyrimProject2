#pragma once

#include "Monster.h"

BEGIN(Engine)

class CCollider;

END

BEGIN(Client)

class CFalmer_UnEquip final : public CMonster
{
public:
	enum FALMERUE_COLTYPE	{ FALMERUE_COL_AABB, 
		FALMERUE_COL_DETECTION, FALMERUE_COL_MISSDETECTION,
		FALMERUE_COL_ATKROUND, FALMERUE_COL_END };
	
	enum FALMERUE_STATE { FALMERUE_SQUAT, FALMERUE_SQUAT_OUTRO, FALMERUE_SQUAT_INTRO,
		FALMERUE_WARNING, FALMERUE_DETECTION, FALMERUE_RETURN,
		FALMERUE_CHASE, 
		FALMERUE_RUNATK,
		FALMERUE_END };

private:
	CFalmer_UnEquip(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFalmer_UnEquip(const CFalmer_UnEquip& rhs);
	virtual ~CFalmer_UnEquip() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // 원본
	virtual HRESULT Initialize_Clone(void* pArg); // 사본
	virtual HRESULT Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg); // 사본
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Set_State(CFalmer_UnEquip::FALMERUE_STATE _eState);

public:
	HRESULT Ready_Component(_uint _iLevel);
	HRESULT Ready_State();

private:
	vector<CCollider*> m_pVecCollider;
	
	class CStateManager_FalmerUE* m_pStateManager = nullptr;

public:
	static  CFalmer_UnEquip* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual CGameObject* Clone(_uint _iLevel, const wstring& _strModelComTag, void* _pArg) override;
	virtual void Free() override;

};

END

