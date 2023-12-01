#include "VIBuffer_Rect_Instance.h"

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer_Instancing(_pDevice, _pContext)
{
}

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& rhs)
	: CVIBuffer_Instancing(rhs)
{
}

HRESULT CVIBuffer_Rect_Instance::Initialize_ProtoType(const INSTANCE_DESC& _InstanceDesc)
{
	/* �ν��Ͻ��� ������. ��. */
	m_iNumInstance = _InstanceDesc.iNumInstance;
	m_iNumIndicesPerInstance = 6;

	/* �׸���� ������ �����ϴ� ��Ʈ. */
	m_iStride = sizeof(VTXPOSTEX); /* �����ϳ��� ũ�� .*/
	m_iNumVertices = 4;

	m_iIndexStride = 2; /* �ε��� �ϳ��� ũ��. 2 or 4 */
	m_iNumIndices = 6 * m_iNumInstance;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iNumVBs = 2;

#pragma region VERTEX_BUFFER

	/* �������ۿ� �ε��� ���۸� �����. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	// m_BufferDesc.ByteWidth = �����ϳ��� ũ��(Byte) * ������ ����;
	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* �������۷� �Ҵ��Ѵ�. (Lock, unLock ȣ�� �Ұ�)*/
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER

	/* �������ۿ� �ε��� ���۸� �����. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* �������۷� �Ҵ��Ѵ�. (Lock, unLock ȣ�� �Ұ�)*/
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	_uint		iNumIndices = 0;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 1;
		pIndices[iNumIndices++] = 2;

		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 2;
		pIndices[iNumIndices++] = 3;
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

#pragma region INSTANCE_BUFFER

	if (FAILED(__super::Initialize_ProtoType(_InstanceDesc)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Initialize_Clone(void* _pArg)
{
#pragma region INSTANCE_BUFFER

	if (FAILED(__super::Initialize_Clone(_pArg)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Update(_float _fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource = {};

	/* ��ƼŬ�� �������� �ο��Ѵ�. */
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_iNumInstance; ++i)
	{
		m_pTimeAccAry[i] += _fTimeDelta;

		((VTXINSTANCE*)SubResource.pData)[i].vTranslation.y += m_pSpeedAry[i] * _fTimeDelta;
	
		if (m_pTimeAccAry[i] >= m_pLifeTimeAry[i])
		{
			((VTXINSTANCE*)SubResource.pData)[i].vTranslation.y = m_pVertices[i].vTranslation.y;
			m_pTimeAccAry[i] = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Update_Spark(_float _fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource = {};

	/* ��ƼŬ�� �������� �ο��Ѵ�. */
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_iNumInstance; ++i)
	{
		// ������ ������ ���� ������Ʈ
		// _float randomAngle = static_cast<_float>(std::rand()) / RAND_MAX * 2.0f * 3.14159f;
		// _float directionZ = std::cos(randomAngle);
		// _float directionY = std::sin(randomAngle);

		m_pTimeAccAry[i] += _fTimeDelta;

		// ���� ���Ϳ� �ӵ��� ���ϰ� ���� ��ġ�� ���ϱ�
		// _float speed = m_pSpeedAry[i];
		// ((VTXINSTANCE*)SubResource.pData)[i].vTranslation.z += speed * directionZ * _fTimeDelta;

		if (m_pTimeAccAry[i] >= m_pLifeTimeAry[i])
		{
			// ������ ���� ��� �ش� ���ڸ� ���� (���ڸ� �׳� ���ֱ�)
			continue;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return S_OK;
}

CVIBuffer_Rect_Instance* CVIBuffer_Rect_Instance::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const INSTANCE_DESC& _InstanceDesc)
{
	CVIBuffer_Rect_Instance* pInstance = new CVIBuffer_Rect_Instance(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType(_InstanceDesc)))
	{
		MSG_BOX("Fail Create : CVIBuffer_Rect_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Rect_Instance::Clone(void* _pArg)
{
	CVIBuffer_Rect_Instance* pInstance = new CVIBuffer_Rect_Instance(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CVIBuffer_Rect_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect_Instance::Free()
{
	__super::Free();
}
