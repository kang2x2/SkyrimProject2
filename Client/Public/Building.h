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

class CBuilding final : public CGameObject
{
private:
	CBuilding(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBuilding(const CBuilding& rhs);
	virtual ~CBuilding() = default;

public:
	virtual HRESULT Initialize_ProtoType() override; // 원본
	virtual HRESULT Initialize_Clone(void* pArg) override; // 사본
	virtual HRESULT Initialize_Clone(const wstring& _strModelComTag, void* pArg) override; // 사본
	virtual void	Tick(_float _fTimeDelta) override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	/* 파일 저장과 로드를 담당하는 함수들 */
	virtual HRESULT Object_FileSave(ofstream& outFile) const;
	virtual HRESULT Object_FileLoad(std::ifstream& inFile);

private:
	CModel* m_pModelCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;

private:
	HRESULT Ready_Component();
	HRESULT Bind_ShaderResource();

public:
	static CBuilding* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual CGameObject* Clone(const wstring& _strModelComTag, void* _pArg) override;
	virtual void Free() override;

};

END

