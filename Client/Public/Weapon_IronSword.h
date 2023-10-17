#pragma once

#include "Client_Defines.h"
#include "Part_Base.h"

BEGIN(Engine)

class CBone;
class CModel;
class CRenderer;
class CShader;
class CTransform;

END

BEGIN(Client)

class CWeapon_IronSword final : public CPart_Base
{
public:
	typedef struct tagPartDesc : public CPart_Base::PART_DESC
	{
		CBone*		pSocketBone = nullptr;
		_float4x4	matSocketPivot;
	}WEAPON_DESC;

private:
	CWeapon_IronSword(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CWeapon_IronSword(const CWeapon_IronSword& rhs);
	virtual ~CWeapon_IronSword() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // 원본
	virtual HRESULT Initialize_Clone(void* pArg); // 사본
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Ready_Component();
	HRESULT Bind_ShaderResources();

private:
	CBone*		m_pSocketBone =  nullptr;
	_float4x4	m_matSocketPivot;

public:
	static CWeapon_IronSword* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

