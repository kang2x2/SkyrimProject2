#pragma once

#include "SkyrimHelmet.h"

BEGIN(Engine)

class CBone;
class CModel;
class CRenderer;
class CShader;
class CTransform;

END

BEGIN(Client)

class CHelmet_Glass final : public CSkyrimHelmet
{
private:
	CHelmet_Glass(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CHelmet_Glass(const CHelmet_Glass& rhs);
	virtual ~CHelmet_Glass() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // ����
	virtual HRESULT Initialize_Clone(void* pArg); // �纻
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
	static  CHelmet_Glass* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

