#include "Model.h"

#include "Mesh.h"

CModel::CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_vMesh(rhs.m_vMesh)
{
	for (auto& iter : m_vMesh)
	{
		Safe_AddRef(iter);
	}
}

HRESULT CModel::Initialize_ProtoType(const char* _strModleFilePath)
{
	// fbx������ �о� �����͸� m_pAIScene���� ����.
	/*  aiProcess_PreTransformVertices 
		�޽��� �̸��� ���� �̸��� ���� �ִ��� �˻��Ѵ�.
		�ִٸ� �ε� �� �� �� ���� ����� ������ �޽��� ��� ������ �����ش�.
		���� ���� �ҷ��� ������ ����� ������ �ִϸ��̼��� �����ϴ� ���� ���� �ε��� ��
		�̹� ������ ���� ���·� �ε��ϰ� �ִϸ��̼� ������ ���� �ε��Ѵ�.*/

	// aiProcess_ConvertToLeftHanded : �޼� ��ǥ��� �˾Ƽ� ��ȯ�Ͽ� ��������(FBX SDK�� ���� ����)
	// aiProcessPreset_TargetRealtime_Fast : �ɼ��� ������ �����ν� ����Ƽ�� ���̰� �ִ�.
	m_pAIScene = m_Importer.ReadFile(_strModleFilePath, aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);
	if (m_pAIScene == nullptr)
		return E_FAIL;

	// DirectX�� �׷��� �� �ְ� �����͸� �����ϴ� �κ�
	
	if (FAILED(Ready_Mesh()))
		return E_FAIL;
		
	return S_OK;
}

HRESULT CModel::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Render()
{
	for (auto& iter : m_vMesh)
	{
		if (iter != nullptr)
		{
			iter->Render();
		}
	}

	return S_OK;
}

HRESULT CModel::Ready_Mesh()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	m_vMesh.reserve(m_iNumMeshes);

	for (size_t i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_pAIScene->mMeshes[i]);
		if (pMesh == nullptr)
			return E_FAIL;

		m_vMesh.push_back(pMesh);
	}

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const char* _strModleFilePath)
{
	CModel* pInstance = new CModel(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType(_strModleFilePath)))
	{
		MSG_BOX("Fail Create : CModel ProtoType");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* _pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Cloen : CModel Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& iter : m_vMesh)
	{
		Safe_Release(iter);
	}
	m_vMesh.clear();

	m_Importer.FreeScene();
}
