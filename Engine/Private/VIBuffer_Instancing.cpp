#include "VIBuffer_Instancing.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs)
	: CVIBuffer(rhs)
	, m_iStrideInstance(rhs.m_iStrideInstance)
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_iNumIndicesPerInstance(rhs.m_iNumIndicesPerInstance)
	, m_pVBInstance(rhs.m_pVBInstance)
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Instancing::Initialize_ProtoType()
{
	m_iStrideInstance = sizeof(VTXINSTANCE);

	/* �������ۿ� �ε��� ���۸� �����. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	// m_BufferDesc.ByteWidth = �����ϳ��� ũ��(Byte) * ������ ����;
	m_BufferDesc.ByteWidth = m_iStrideInstance * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* �������۷� �Ҵ��Ѵ�. (Lock, unLock ȣ�� ����)*/
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStrideInstance;

	VTXINSTANCE* pVertices = new VTXINSTANCE[m_iNumInstance];
	ZeroMemory(pVertices, sizeof(VTXINSTANCE) * m_iNumInstance);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pVertices[i].vTranslation = _float4(rand() % 5, rand() % 5, rand() % 5, 1.f);
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Update(_float _fTimeDelta)
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Render()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint			iStrides[] = {
		m_iStride,
		m_iStrideInstance,

	};

	_uint			iOffsets[] = {
		0,
		0,
	};

	/* ���ؽ� ���۵��� �Ҵ��Ѵ�. */
	/* �׸���� �������� + ���º�ȯ�� �������� */
	m_pContext->IASetVertexBuffers(0, m_iNumVBs, pVertexBuffers, iStrides, iOffsets);

	/* �ε��� ���۸� �Ҵ��Ѵ�. */
	/* �׸����� �ϴ� �ν��Ͻ��� ������ŭ Ȯ��Ǿ��ִ� �ε��� ����. */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	/* �ش� �������� � ������� �׸�����. */
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	/* �ε����� ����Ű�� ������ Ȱ���Ͽ� �׸���. */
	m_pContext->DrawIndexedInstanced(m_iNumIndicesPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Instancing::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
}
