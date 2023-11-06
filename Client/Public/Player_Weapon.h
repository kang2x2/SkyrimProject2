#pragma once

#include "Client_Defines.h"
#include "Part_Base.h"
#include "SkyrimWeapon.h"

BEGIN(Engine)

class CBone;
class CRenderer;
class CTransform;
class CCollider;

END

BEGIN(Client)

class CPlayer_Weapon final : public CPart_Base
{
public:
	typedef struct tagPartDesc : public CPart_Base::PART_DESC
	{
		CBone*		pSocketBone = nullptr;
		_float4x4	matSocketPivot;
	}WEAPON_DESC;

private:
	CPlayer_Weapon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayer_Weapon(const CPlayer_Weapon& rhs);
	virtual ~CPlayer_Weapon() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // 원본
	virtual HRESULT Initialize_Clone(void* _pArg); // 사본
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void		Set_SoketBone(CBone* _pBone);
	const char* Get_SoketBoneName();

public:
	HRESULT Ready_Component();
	HRESULT Bind_ShaderResources();

private:
	CCollider*		m_pColliderCom = nullptr;

	CBone*			m_pSocketBone = nullptr;
	_float4x4		m_matSocketPivot;

	CGameObject*	m_pWeapon = nullptr;


public:
	static CPlayer_Weapon* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

};

END
