#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CLoadingObj_Skeever : public CGameObject
{
private:
	CLoadingObj_Skeever(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CLoadingObj_Skeever(const CLoadingObj_Skeever& rhs);
	virtual ~CLoadingObj_Skeever() = default;

public:
	virtual HRESULT Initialize_ProtoType() override; // ����
	virtual HRESULT Initialize_Clone(void* pArg) override; // �纻
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta) override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel* m_pModelCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;

	wstring		m_strText = TEXT("");

private:
	HRESULT Ready_Component(void* pArg);
	HRESULT Bind_ShaderResource();

public:
	static CLoadingObj_Skeever* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

