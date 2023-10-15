#include "Mesh.h"

#include "Shader.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CMesh::Initialize_ProtoType(const CModel* _pModel, const CBin_AIScene::DESC_MESH* _pAIMesh, _fmatrix _matPivot, CModel::MODEL_TYPE _eType)
{
	strcpy_s(m_szName, _pAIMesh->mName.data);

	m_iStride = CModel::TYPE_NONANIM ==  _eType ? sizeof(VTX_NONANIMMESH) : sizeof(VTX_ANIMMESH);

	/* �� �޽ô� mMaterialIndex��° ���׸��� ������ �̿��Ѵ�. */
	m_iMaterialIndex = _pAIMesh->mMaterialIndex;
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

	HRESULT hr = CModel::TYPE_NONANIM == _eType ?
		Ready_VertexBuffer_For_NonAnim(_pAIMesh, _matPivot) :
		Ready_VertexBuffer_For_Anim(_pModel, _pAIMesh);

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region Index Buffer

	/* �������ۿ� �ε��� ���۸� �����. */
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

HRESULT CMesh::Bind_BondMatrices(CShader* _pShader, vector<class CBone*>& _vecBone, const char* _strConstantName, _fmatrix PivotMatrix)
{
	_float4x4 BoneMatrices[256];
	ZeroMemory(BoneMatrices, sizeof(_float4x4) * 256);

	// ���� ���� ��� * �θ� �ε��� �� ��İ� �� �ڽ��� ����� ���� ���.
	//for (size_t i = 0; i < m_vecBoneIndex.size(); ++i)
	//{
	//	_float4x4 matBone = _vecBone[m_vecBoneIndex[i]]->Get_CombinedTransformationMatrix();
	//	XMStoreFloat4x4(&BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * XMLoadFloat4x4(&matBone));
	//	//  * XMMatrixScaling(0.0025f, 0.005f, 0.005f)
	//}

	_uint		iIndex = 0;

	for (auto iBoneIndex : m_vecBoneIndex)
	{
		_float4x4 matBone = _vecBone[iBoneIndex]->Get_CombinedTransformationMatrix();

		XMStoreFloat4x4(&BoneMatrices[iIndex++],
			XMLoadFloat4x4(&m_OffsetMatrices[iIndex]) * XMLoadFloat4x4(&matBone) * PivotMatrix);
	}


	return _pShader->Bind_Matrices(_strConstantName, BoneMatrices, m_iNumBones);
}

void CMesh::Update_VI(const _fmatrix& _matPivot)
{
	VTX_NONANIMMESH* pVertices = new VTX_NONANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTX_NONANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), _matPivot));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), _matPivot));
	}

	Safe_Delete_Array(pVertices);
}

HRESULT CMesh::Ready_VertexBuffer_For_NonAnim(const CBin_AIScene::DESC_MESH* _pAIMesh, _fmatrix _matPivot)
{
	VTX_NONANIMMESH* pVertices = new VTX_NONANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTX_NONANIMMESH) * m_iNumVertices);

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

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_Anim(const CModel* _pModel, const CBin_AIScene::DESC_MESH* _pAIMesh)
{
	VTX_ANIMMESH* pVertices = new VTX_ANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTX_ANIMMESH) * m_iNumVertices);

	m_pPos = new _float3[m_iNumVertices];

	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &_pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &_pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &_pAIMesh->mTangents[i], sizeof(_float3));

		m_pPos[i] = pVertices[i].vPosition;
	}

	m_iNumBones = _pAIMesh->mNumBones;

	/* ���κ��� � �������� ���� ä�����ϴ����� �˾ƺ���? */
	/* ������ ������ � �����鿡�� ������ �ִ°�? */
	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		// �� ���� ��������.
		CBin_AIScene::DESC_MESHBONE pAIBone = _pAIMesh->mBones[i];

		/*  �������� ���Ǵ� ������� ����ٸ� ������ �޽ÿ��� �����ϱ����ؼ���
			��������� ������ �ʿ��ϴ�. */
		// offset : ������ �����ϴ� � ��, ����ġ Ȥ�� ġ�� ����.
		_float4x4 matOffset; // ���� ��� ����
		memcpy(&matOffset, &pAIBone.mOffsetMatrix, sizeof(_float4x4)); // ���� offset��� ����.
		XMStoreFloat4x4(&matOffset, XMMatrixTranspose(XMLoadFloat4x4(&matOffset))); // ��ġ

		m_OffsetMatrices.push_back(matOffset); // Push

		_int iBoneIndex = _pModel->Get_BoneIndex(pAIBone.mName.data);
		if (iBoneIndex == -1)
			return E_FAIL;

		m_vecBoneIndex.push_back(iBoneIndex);

		/* pAIBone->mNumWeights : �� ���� ��� �����鿡�� ������ �ִ°�? */
		for (size_t j = 0; j < pAIBone.mNumWeights; ++j)
		{
			/*  pAIBone->mWeights[j].mVertexId : i��° ���� j��° ������
				pAIBone->mWeights[j].mVertexId ��° �������� ������ �ش�.  */
			// ���� ������ ���� �ʾ� 0.f�� ����Ǿ� �ֱ⿡ ������� ��������.(������ �ִ� �ุ)
			if (pVertices[pAIBone.mWeights[j].mVertexId].vBlendWeights.x == 0.f)
			{
				pVertices[pAIBone.mWeights[j].mVertexId].vBlendIndices.x = i;
				pVertices[pAIBone.mWeights[j].mVertexId].vBlendWeights.x = pAIBone.mWeights[j].mWeight;
			}

			else if (pVertices[pAIBone.mWeights[j].mVertexId].vBlendWeights.y == 0.f)
			{
				pVertices[pAIBone.mWeights[j].mVertexId].vBlendIndices.y = i;
				pVertices[pAIBone.mWeights[j].mVertexId].vBlendWeights.y = pAIBone.mWeights[j].mWeight;
			}

			else if (pVertices[pAIBone.mWeights[j].mVertexId].vBlendWeights.z == 0.f)
			{
				pVertices[pAIBone.mWeights[j].mVertexId].vBlendIndices.z = i;
				pVertices[pAIBone.mWeights[j].mVertexId].vBlendWeights.z = pAIBone.mWeights[j].mWeight;
			}

			else if (pVertices[pAIBone.mWeights[j].mVertexId].vBlendWeights.w == 0.f)
			{
				pVertices[pAIBone.mWeights[j].mVertexId].vBlendIndices.w = i;
				pVertices[pAIBone.mWeights[j].mVertexId].vBlendWeights.w = pAIBone.mWeights[j].mWeight;
			}
		}
	}

	/* �� ���� �ʿ��ѵ� �� ���� 0�� �� */
	if (m_iNumBones == 0)
	{
		m_iNumBones = 1; // ������ �÷��ش�.

		_uint iIndex = _pModel->Get_BoneIndex(m_szName);
		if (iIndex == -1)
			return E_FAIL;

		_float4x4 matOffset;
		XMStoreFloat4x4(&matOffset, XMMatrixIdentity());

		m_OffsetMatrices.push_back(matOffset);

		m_vecBoneIndex.push_back(iIndex);
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const CModel* _pModel, const CBin_AIScene::DESC_MESH* _pAIMesh, _fmatrix _matPivot, CModel::MODEL_TYPE _eType)
{
	CMesh* pInstance = new CMesh(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType(_pModel, _pAIMesh, _matPivot, _eType)))
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
