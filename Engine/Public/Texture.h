#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_ProtoType(const wstring& _strTextureFilePath, _uint _iNumTextures);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	// �ؽ�ó �� �� ���̴��� ����
	HRESULT Bind_ShaderResource(const class CShader* _pShader, const char* _pConstantName, _uint _iNumTextures);
	// �迭�� ������ ��� �ؽ�ó ���̴��� ����
	HRESULT Bind_ShaderResources(const class CShader* _pShader, const char* _pConstantName);

private:
	// ���̴� �ܰ迡�� ���ҽ��� ���ε� �ϱ� ���� �ʿ�.
	ID3D11ShaderResourceView** m_ppSRVs = nullptr; // ���� �迭
	_uint					   m_iNumTextures = 0;

public:
	static CTexture* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		const wstring& _strTextureFilePath, _uint _iNumTextures);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

