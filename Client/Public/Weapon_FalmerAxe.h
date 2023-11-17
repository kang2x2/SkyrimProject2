#pragma once

#include "SkyrimWeapon.h"

BEGIN(Client)

class CWeapon_FalmerAxe final : public CSkyrimWeapon
{
private:
	CWeapon_FalmerAxe(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CWeapon_FalmerAxe(const CWeapon_FalmerAxe& rhs);
	virtual ~CWeapon_FalmerAxe() = default;

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
	CModel* m_pModelCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTransform* m_pTransformCom = nullptr;

	CBone* m_pSocketBone = nullptr;
	_float4x4	m_matSocketPivot;

public:
	static  CWeapon_FalmerAxe* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

};

END

