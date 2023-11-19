#pragma once

#include "CarlottaPart_Base.h"
#include "SkyrimClothes.h"

BEGIN(Engine)

class CBone;
class CRenderer;
class CTransform;
class CCollider;

END

BEGIN(Client)

class CCarlotta_Body final : public CCarlottaPart_Base
{
public:
	typedef struct tagPartDesc : public CCarlottaPart_Base::PART_DESC
	{
		CBone*		pSocketBone = nullptr;
		_float4x4	matSocketPivot;
	}BODY_DESC;

private:
	CCarlotta_Body(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCarlotta_Body(const CCarlotta_Body& rhs);
	virtual ~CCarlotta_Body() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // 원본
	virtual HRESULT Initialize_Clone(void* _pArg); // 사본
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void		Set_PivotMatrix(_float4x4 _matPivot) { m_matSocketPivot = _matPivot; }
	const char* Get_SoketBoneName();

public:
	HRESULT Ready_Component();
	HRESULT Bind_ShaderResource();

private:
	CCollider*		m_pColliderCom = nullptr;

	CBone*			m_pSocketBone = nullptr;
	_float4x4		m_matSocketPivot;

	CGameObject* m_pBody = nullptr;


public:
	static  CCarlotta_Body* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

