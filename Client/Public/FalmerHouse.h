#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CModel;
class CShader;
class CRenderer;
class CTransform;
class CCollider;

END

BEGIN(Client)

class CFalmerHouse final : public CGameObject
{
private:
	CFalmerHouse(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFalmerHouse(const CFalmerHouse& rhs);
	virtual ~CFalmerHouse() = default;

public:
	virtual HRESULT Initialize_ProtoType() override; // 원본
	virtual HRESULT Initialize_Clone(void* pArg) override; // 사본
	virtual HRESULT Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg) override; // 사본
	virtual void	Tick(_float _fTimeDelta) override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel* m_pModelCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CCollider* m_pColliderCom = nullptr;

private:
	HRESULT Ready_Component(_uint _iLevel);
	HRESULT Bind_ShaderResource();

public:
	static CFalmerHouse* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual CGameObject* Clone(_uint _iLevel, const wstring& _strModelComTag, void* _pArg) override;
	virtual void Free() override;
};

END
