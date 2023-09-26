#include "Shader.h"

CShader::CShader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_vecInputLayOut(rhs.m_vecInputLayOut)
{
	for (auto& iter : m_vecInputLayOut)
		Safe_AddRef(iter);

	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_ProtoType(const wstring& _strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* _pElements, _uint iNumElements)
{
	_uint iHlslFlag = 0;

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	/* �ش� ��ο� ���̴� ������ �������Ͽ� ��üȭ�Ѵ�. */
	if (FAILED(D3DX11CompileEffectFromFile(_strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	/* �� ���̴��ȿ� �ִ� ��� �н��� ���� �׸������ϴ� ������ �� �޾��� �� �ִ°�?! Ȯ��. �̰� Ȯ�εǸ� */
	/* ID3D11InputLayout�� ������ �ش�. */
	/* ��������� ID3D11InputLayout�� �������ֱ����� ���� �۾��� ���� �Ѵ� .*/

	/* �н��� ������� �۾��� �����Ѵ�. �н��� ��ũ��Ŀ �ȿ� ����Ǿ��ִ�. */
	/* �� ���̴� �ȿ� ���ǵ� ��ũ��Ŀ�� ������ ���´�. (���� ��ũ��Ŀ�� �ϳ��� �����߱⶧���� 0��° �ε����� �޾ƿ°ſ�. )*/
	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (pTechnique == nullptr)
		return E_FAIL;

	/* ��ũ��Ŀ�� ������ ���´�. */
	D3DX11_TECHNIQUE_DESC	TechniqueDesc;
	pTechnique->GetDesc(&TechniqueDesc);

	/* TechniqueDesc.Passes : �н��� ����. */
	for (_uint i = 0; i < TechniqueDesc.Passes; ++i)
	{
		/* pTechnique->GetPassByIndex(i) : i��° �н���ü�� ���´�.
		ID3DX11EffectPass : ��� ������ ���� �Ҵ��� ĸ��ȭ*/
		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);

		/* �н��� ������ ���´�. */
		D3DX11_PASS_DESC		PassDesc;
		pPass->GetDesc(&PassDesc);

		ID3D11InputLayout* pInputLayout = nullptr;

		/* �н��ȿ� ����� ���������� ���� ������ ���������� ��ġ�Ѵٸ� ID3D11InputLayout�� �������ش�. */
		if (FAILED(m_pDevice->CreateInputLayout(_pElements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
			return E_FAIL;

		m_vecInputLayOut.push_back(pInputLayout);
	}

	return S_OK;
}

HRESULT CShader::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CShader::Begin(_uint _iPassIndex)
{
	if (_iPassIndex >= m_vecInputLayOut.size())
		return E_FAIL;

	// �Է� ���̾ƿ� ��ü�� �Է�. ����� �ܰ迡 ���ε�.
	m_pContext->IASetInputLayout(m_vecInputLayOut[_iPassIndex]);

	// ���(Technique)�� �׽�Ʈ�Ͽ� ��ȿ�� ������ ���ԵǾ� �ִ��� Ȯ��.(�ϳ��� ���Ŷ� ������ 0�� �ε���)
	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (pTechnique == nullptr)
		return E_FAIL;

	/* iPassIndex �� ° �н��� �׸��⸦ ����. 
	   �ݵ�� �׸��� ���� �ؽ��Ŀ��� ������ ������ ��� �����ϰ� �׷��� �Ѵ�. */
	pTechnique->GetPassByIndex(_iPassIndex)->Apply(0, m_pContext);

	return S_OK;
}

HRESULT CShader::Bind_RawValue(const char* _pConstantName, const void* _pData, _uint _iLength)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(_pData, 0, _iLength);
}

HRESULT CShader::Bind_Matrix(const char* _pConstantName, const _float4x4* _pMatrix) const
{
	/* pConstantName�̸��� �ش��ϴ� Ÿ���� ����������� ���������� ��Ʈ���ϴ� ��ü�� ���´�. 
	   ID3DX11EffectVariable : ��� ȿ�� ���� Ŭ������ Base Ŭ����. */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
	if (pVariable == nullptr)
		return E_FAIL;

	// ��Ŀ� ����.
	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (pMatrixVariable == nullptr)
		return E_FAIL;
	
	return pMatrixVariable->SetMatrix((const _float*)_pMatrix);
}

HRESULT CShader::Bind_Matrices(const char* _pConstantName, const _float4x4* _pMatrices, _uint _iNumMatrices) const
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
	if (pVariable == nullptr)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrix = pVariable->AsMatrix();
	if (pMatrix == nullptr)
		return E_FAIL;

	return pMatrix->SetMatrixArray((_float*)_pMatrices, 0, _iNumMatrices);
}

HRESULT CShader::Bind_Texture(const char* _pConstantName, ID3D11ShaderResourceView* _pSRV) const
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
	if (pVariable == nullptr)
		return E_FAIL;

	// ID3DX11EffectShaderResourceVariable : ���̴� ���ҽ��� ���õ� ID3DX11EffectVariable���� ��ӵ� �������̽�.
	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (pSRVariable == nullptr)
		return E_FAIL;

	return pSRVariable->SetResource(_pSRV);
}

HRESULT CShader::Bind_Textures(const char* _pConstantName, ID3D11ShaderResourceView** _ppSRV, _uint _iNumTextures) const
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
	if (pVariable == nullptr)
		return E_FAIL;

	// ID3DX11EffectShaderResourceVariable : ���̴� ���ҽ��� ���õ� ID3DX11EffectVariable���� ��ӵ� �������̽�.
	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (pSRVariable == nullptr)
		return E_FAIL;

	return pSRVariable->SetResourceArray(_ppSRV, 0, _iNumTextures);
}

CShader* CShader::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _strShaderFilePath
	, const D3D11_INPUT_ELEMENT_DESC* _pElements, _uint _iNumElements)
{
	CShader* pInstance = new CShader(_pDevice, _pContext);

 	if (FAILED(pInstance->Initialize_ProtoType(_strShaderFilePath, _pElements, _iNumElements)))
	{
		MSG_BOX("Fail Create : CShader_ProtoType");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
	CShader* pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX("Fail Clone : CShader_Clone");
		Safe_Release(pInstance);
	}

	return pInstance;

}

void CShader::Free()
{
	__super::Free();

	for (auto& iter : m_vecInputLayOut)
	{
		Safe_Release(iter);
	}

	Safe_Release(m_pEffect);
}
