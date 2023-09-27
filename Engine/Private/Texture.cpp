#include "Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CTexture::CTexture(const CTexture& rhs)
	: CComponent(rhs)
	, m_iNumTextures(rhs.m_iNumTextures)
	, m_ppSRVs(rhs.m_ppSRVs)
{
	for (_uint i = 0; i < m_iNumTextures; ++i)
	{
		Safe_AddRef(m_ppSRVs[i]);
	}
}

HRESULT CTexture::Initialize_ProtoType(const wstring& _strTextureFilePath, _uint _iNumTextures)
{
	// �ؽ�ó �� ����.
	m_iNumTextures = _iNumTextures;

	// �ڿ�(�ؽ�ó)�� ���� ���� �迭 �Ҵ�.
	m_ppSRVs = new ID3D11ShaderResourceView*[_iNumTextures];

	// �ؽ�ó�� �� ��ŭ �ݺ����� ���� �ؽ�ó�� ����.
	for (size_t i = 0; i < _iNumTextures; ++i)
	{
		// ��ü ���� ��� ������ ���� ���ڿ� ���� ����
		_tchar szTextureFilePath[MAX_PATH] = TEXT("");

		// ���� ��� ����
 		wsprintf(szTextureFilePath, _strTextureFilePath.c_str(), i);

		// ���� Ȯ���� ������ ���� ����(dds, jpg, png, bmp)
		_tchar szExt[MAX_PATH] = TEXT("");

		// _wsplitpath_s : ��� �̸��� ���� ��ҷ� ����.
		// ���� �� �ξ��� ��ü ���ϸ��� ���� ��ҷ� �����Ͽ� szExt�� ��ڴ�.
		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		HRESULT hr = 0;
		ID3D11ShaderResourceView* pSRV = nullptr; // �迭�� ���� ���ҽ� ����.

		// ���� Ȯ���ڿ� ���� ó��.(strcmp�� ������ 0�� ��ȯ��)
		if (lstrcmp(szExt, TEXT(".dds")) == false)
		{
			hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
		}
		else if (lstrcmp(szExt, TEXT(".tga")) == false)
		{
			return S_OK;
		}
		else
		{
			hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
		}

		if (FAILED(hr))
			return E_FAIL;

		// ���������� �ؽ�ó�� �����Ͽ����� ����.
		m_ppSRVs[i] = pSRV;
	}

	return S_OK;
}

HRESULT CTexture::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(const CShader* _pShader, const char* _pConstantName, _uint _iNumTextures)
{
	return _pShader->Bind_Texture(_pConstantName, m_ppSRVs[_iNumTextures]);
}

HRESULT CTexture::Bind_ShaderResources(const CShader* _pShader, const char* _pConstantName)
{
	return _pShader->Bind_Textures(_pConstantName, m_ppSRVs, m_iNumTextures);
}

CTexture* CTexture::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _strTextureFilePath, _uint _iNumTextures)
{
	CTexture* pInstance = new CTexture(_pDevice, _pContext);

 	if (FAILED(pInstance->Initialize_ProtoType(_strTextureFilePath, _iNumTextures)))
	{
		MSG_BOX("Fail Create : CTexture ProtoType");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX("Fail Clone : CTexture Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumTextures; ++i)
	{
		Safe_Release(m_ppSRVs[i]);
	}

	// ������ ������ ����ڴ�. ��۸� ������ ����
	// �纻�� �� ����� �Ǹ� ���� ���� ����� ���� ���̱⿡ ������ ���� ���� ������ ��찡 ����
	// ���α׷��� ������ ����� �� ����.
	if (m_isCloned == false)
		Safe_Delete_Array(m_ppSRVs);
}
