#include "Mesh.h"

CMesh::CMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CMesh::Initialize_ProtoType(const aiMesh* _pAIMesh, _fmatrix _matPivot)
{
	/* �� �޽ô� mMaterialIndex��° ���׸��� ������ �̿��Ѵ�. */
	m_iMaterialIndex = _pAIMesh->mMaterialIndex;

	m_iStride = sizeof(VTXMESH);
	m_iNumVertices = _pAIMesh->mNumVertices;
	m_iIndexStride = 4;
	m_iNumIndices = _pAIMesh->mNumFaces * 3;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iNumVBs = 1;

#pragma region Vertex Buffer
	
	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	m_pPos = new _float3[m_iNumVertices];

	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		/*  ���� �غ� ���� Pivot ����� �޾ƿԴ�. 
			�� ����� ���� ���� ���ϴ� ��ġ, ȸ��, �������� ���Ϳ� ���� ������ �� �� �ִ�.*/
		memcpy(&pVertices[i].vPosition, &_pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), _matPivot));
		
		// ���� ���� ���
		m_pPos[i] = pVertices[i].vPosition;

		/* ��ġ, ȸ��, �������� �������Ƿ� normal�� �Ȱ��� ��ȯ�� �ʿ��ϴ�.*/
		memcpy(&pVertices[i].vNormal, &_pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), _matPivot));

		memcpy(&pVertices[i].vTexcoord, &_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &_pAIMesh->mTangents[i], sizeof(_float3));
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region Index Buffer

	/* �������ۿ� �ε��� ���۸� ���峮. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* �������۷� �Ҵ��Ѵ�. (Lock, unLock ȣ�� �Ұ�)*/
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint		iNumIndices = 0;

	for (size_t i = 0; i < _pAIMesh->mNumFaces; i++)
	{
		pIndices[iNumIndices++] = _pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = _pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = _pAIMesh->mFaces[i].mIndices[2];

		m_vecIndex.push_back(_pAIMesh->mFaces[i].mIndices[0]);
		m_vecIndex.push_back(_pAIMesh->mFaces[i].mIndices[1]);
		m_vecIndex.push_back(_pAIMesh->mFaces[i].mIndices[2]);
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize_Clone(void* _pArg)
{
	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const aiMesh* _pAIMesh, _fmatrix _matPivot)
{
	CMesh* pInstance = new CMesh(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType(_pAIMesh, _matPivot)))
	{
		MSG_BOX("Fail Create : CMesh ProtoType");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMesh::Clone(void* _pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CMesh Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pPos);
	m_vecIndex.clear();
}
