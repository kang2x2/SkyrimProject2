#pragma once

#include "Client_Defines.h"
#include "FalmerOHPart_Base.h"

BEGIN(Engine)

class CBone;
class CRenderer;
class CTransform;
class CCollider;

END

BEGIN(Client)

class CFalmerOH_Weapon final : public CFalmerOHPart_Base
{
public:
	typedef struct tagPartDesc : public CFalmerOH_Weapon::PART_DESC
	{
		CBone* pSocketBone = nullptr;
		_float4x4	matSocketPivot;
	}WEAPON_DESC;

private:
	CFalmerOH_Weapon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFalmerOH_Weapon(const CFalmerOH_Weapon& rhs);
	virtual ~CFalmerOH_Weapon() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // ����
	virtual HRESULT Initialize_Clone(void* _pArg); // �纻
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void		Set_PivotMatrix(_float4x4 _matPivot) { m_matSocketPivot = _matPivot; }
	const char* Get_SoketBoneName();
	void		Set_SoketBone(CBone* _pSocketBone);

public:
	HRESULT Ready_Component();

private:
	CCollider* m_pColliderCom = nullptr;

	CBone* m_pSocketBone = nullptr;
	_float4x4		m_matSocketPivot;

	CGameObject* m_pWeapon = nullptr;

public:
	static CFalmerOH_Weapon* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END


