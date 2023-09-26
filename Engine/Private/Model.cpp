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
	// fbx파일을 읽어 데이터를 m_pAIScene으로 리턴.
	/*  aiProcess_PreTransformVertices 
		메시의 이름과 같은 이름의 뼈가 있는지 검사한다.
		있다면 로드 할 때 그 뼈의 행렬을 가져와 메시의 모든 정점에 곱해준다.
		정적 모델을 불러올 때에는 상관이 없으나 애니메이션이 존재하는 동적 모델을 로드할 때
		이미 연산이 끝난 상태로 로드하고 애니메이션 정보를 빼고 로드한다.*/

	// aiProcess_ConvertToLeftHanded : 왼손 좌표계로 알아서 변환하여 가져와줌(FBX SDK는 지원 안함)
	// aiProcessPreset_TargetRealtime_Fast : 옵션을 변경해 줌으로써 퀄리티의 차이가 있다.
	m_pAIScene = m_Importer.ReadFile(_strModleFilePath, aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);
	if (m_pAIScene == nullptr)
		return E_FAIL;

	// DirectX로 그려낼 수 있게 데이터를 정리하는 부분
	
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
