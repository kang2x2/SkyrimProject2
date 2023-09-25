#include "VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Terrain::Initialize_ProtoType(const wstring& _strHeightMapFilePath)
{
	/* �޾ƿ� ����Ʈ�� �̹����� ������ �о����. */
	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(_strHeightMapFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	BITMAPFILEHEADER		fh;
	BITMAPINFOHEADER		ih;

	ReadFile(hFile, &fh, sizeof(fh), &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof(ih), &dwByte, nullptr);
	
	_ulong* pPixel = { nullptr };
	pPixel = new _ulong[ih.biWidth * ih.biHeight];

	ReadFile(hFile, pPixel, sizeof(_ulong) * ih.biWidth * ih.biHeight, &dwByte, nullptr);

	CloseHandle(hFile);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;
	m_iStride = sizeof(VTXNORTEX); /* �����ϳ��� ũ�� .*/
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_iIndexStride = 4; /* �ε��� �ϳ��� ũ��. 2 or 4 */
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iNumVBs = 1;

#pragma region VERTEX_BUFFER

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;
			
			/*  ȸ���� RGB�� ��� ���� ������ RGB �� ��� �� ����� ������ R�̳� G���� ������
				���� �ʹ� Ŀ���⿡ B�� ���� �����Ѵ�.
				x = j
				y = ��Ʈ �������� ���� ���̿��� �������� ���� ���� ���� ���� ������.
				z = i */
			pVertices[iIndex].vPosition = _float3(j, (pPixel[iIndex] & 0x000000ff) / 10.f, i);

			// ���� ��ų ���̱⿡ 0���� �ʱ�ȭ.
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f); 

			/*  ���� ó���� ����Ǵ� ���� 0,0��ǥ��, ���� ���߿� ����Ǵ� ���� 1,1�� �����Ϸ� �Ѵ�.
				���� �� �� �� ���� 0,0 ���� 1,1 ���� �Ǵ� ������ �� �� ���� ���̴�. */
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}

	Safe_Delete_Array(pPixel);

#pragma endregion

#pragma region INDEX_BUFFER
	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint  iNumIndices = 0;

	for (size_t i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			_uint iIndices[4] =
			{
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			// ���� ���͸� �����ϱ� �����̴�.
			_vector vSourDir, vDestDir, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			// 0���� 1�� ���� ���� ����, 1���� 2�� ���� ���� ���͸� ���ϰ� 
			// 2���� ���� ���͸� �����Ͽ� ������ �Ǵ� ���� ���͸� ������.
			// Store ���������� ġȯ, Load ��������� ġȯ
			vSourDir = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) 
					   - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition)
					   - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNormal =  XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			// XMStoreFloat3 �Լ��� ������ ���� ���� ���͸� ������ ���� ����(���� 0,0,0)�� ������Ű��.
			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, 
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal));


			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			// 0���� 1�� ���� ���� ����, 1���� 2�� ���� ���� ���͸� ���ϰ� 
			// 2���� ���� ���͸� �����Ͽ� ������ �Ǵ� ���� ���͸� ������.
			// Store ���������� ġȯ, Load ��������� ġȯ
			vSourDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition)
				- XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[iIndices[3]].vPosition)
				- XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			// XMStoreFloat3 �Լ��� ������ ���� ���� ���͸� ������ ���� ����(���� 0,0,0)�� ������Ű��.
			XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal));
		}
	}

#pragma endregion

#pragma region Create Buffer
	/* ���� ���۸� �����. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	// m_BufferDesc.ByteWidth = �����ϳ��� ũ��(Byte) * ������ ����;
	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* �������۷� �Ҵ��Ѵ�. (Lock, unLock ȣ�� �Ұ�)*/
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	/* �ε��� ���۸� �����. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* �������۷� �Ҵ��Ѵ�. (Lock, unLock ȣ�� �Ұ�)*/
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;

}

HRESULT CVIBuffer_Terrain::Initialize_Clone(void* pArg)
{
	return S_OK;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _strHeightMapFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType(_strHeightMapFilePath)))
	{
		MSG_BOX("Fail Create : CVIBuffer_Terrain ProtoType");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX("Fail Clone : CVIBuffer_Terrain Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();
}
