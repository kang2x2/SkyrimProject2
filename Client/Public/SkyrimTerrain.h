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

class CSkyrimTerrain final : public CGameObject
{
private:
	CSkyrimTerrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSkyrimTerrain(const CSkyrimTerrain& rhs);
	virtual ~CSkyrimTerrain() = default;

public:
	virtual HRESULT Initialize_ProtoType() override; // 원본
	virtual HRESULT Initialize_Clone(void* pArg) override; // 사본
	virtual HRESULT Initialize_Clone(const wstring& _strModelComTag) override; // 사본
	virtual void	Tick(_float _fTimeDelta) override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel*			m_pModelCom = nullptr;
	CShader*		m_pShaderCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;

	wstring  m_strModelComTag = TEXT("");

private:
	HRESULT Ready_Component(const wstring& _strProtoModelComTag);
	HRESULT Bind_ShaderResource();

public:
	static CSkyrimTerrain* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual CGameObject* Clone(const wstring& _strModelComTag) override;
	virtual void Free() override;

};

END

