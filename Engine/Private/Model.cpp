#include "Model.h"

#include "Mesh.h"
#include "Texture.h"

CModel::CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_vecMesh(rhs.m_vecMesh)
	, m_iNumMaterails(rhs.m_iNumMaterails)
	, m_vecMaterial(rhs.m_vecMaterial)
{
	for (auto& iter : m_vecMaterial)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
		{
			Safe_AddRef(iter.pTextures[i]);
		}
	}

	for (auto& iter : m_vecMesh)
		Safe_AddRef(iter);
}

HRESULT CModel::Initialize_ProtoType(const char* _strModleFilePath, _fmatrix _matPivot)
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

	// matrix 타입을 보관용 float4로 변환
	XMStoreFloat4x4(&m_matPivot, _matPivot);

	// DirectX로 그려낼 수 있게 데이터를 정리하는 부분
	
	if (FAILED(Ready_Mesh()))
		return E_FAIL;
		
	if (FAILED(Ready_Material(_strModleFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Bind_MaterialTexture(CShader* _pShader, const char* _pConstantName, _uint _iMeshIndex, aiTextureType _eType)
{
	if (_iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint iMaterialIndex = m_vecMesh[_iMeshIndex]->Get_MaterialIndex();
	if (iMaterialIndex >= m_iNumMaterails)
		return E_FAIL;

	return m_vecMaterial[iMaterialIndex].pTextures[_eType]->Bind_ShaderResource(_pShader, _pConstantName, 0);
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	// 메시별로 사용하는 재질 정보가 다를 수도 있기에 기존 방법은 폐기
	//for (auto& iter : m_vecMesh)
	//{
	//	if (iter != nullptr)
	//	{
	//		iter->Render();
	//	}
	//}

	m_vecMesh[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Ready_Mesh()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	m_vecMesh.reserve(m_iNumMeshes);

	for (size_t i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_matPivot));
		if (pMesh == nullptr)
			return E_FAIL;

		m_vecMesh.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Material(const char* _pModelFilePath)
{
	// 재질 정보가 몇개인지
	m_iNumMaterails = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterails; ++i)
	{
		aiMaterial* pMaterials = m_pAIScene->mMaterials[i];

		// 초기화
		MESH_MATERIAL MeshMaterial;
		ZeroMemory(&MeshMaterial, sizeof(MeshMaterial));

		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString strTexturePath;

			// 해당 재질 정보가 없으면 다음으로
			if (FAILED(pMaterials->GetTexture(aiTextureType(j), 0, &strTexturePath)))
				continue;

			// 드라이브, 경로 추출
			char			szDrive[MAX_PATH] = "";
			char			szDirectory[MAX_PATH] = "";
			_splitpath_s(_pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			// 파일명.확장자 추출
			char			szFileName[MAX_PATH] = "";
			char			szExt[MAX_PATH] = "";
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			// 드라이브 복사 후 뒤에 경로, 파일명, 확장자를 이어 붙인다.
			char			szTmp[MAX_PATH] = "";
			strcpy_s(szTmp, szDrive);
			strcat_s(szTmp, szDirectory);
			strcat_s(szTmp, szFileName);
			strcat_s(szTmp, szExt);

			// 문자열 타입 변환 필요.
			_tchar			szFullPath[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szTmp, strlen(szTmp), szFullPath, MAX_PATH);

			// 위에서 조합한 경로로 텍스쳐를 불러온다.
			MeshMaterial.pTextures[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath);
			if (nullptr == MeshMaterial.pTextures[j])
				return E_FAIL;
		}

		// 텍스쳐들을 다 저장한 구조체 push.
		m_vecMaterial.push_back(MeshMaterial);
	}

	return S_OK;
}

void CModel::Update_VI(const _fmatrix& _matPivot)
{
	for (size_t i = 0; i < m_vecMesh.size(); ++i)
	{
		m_vecMesh[i]->Update_VI(_matPivot);
	}
}

CModel* CModel::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const char* _strModleFilePath, _fmatrix _matPivot)
{
	CModel* pInstance = new CModel(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType(_strModleFilePath, _matPivot)))
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

	for (auto& iter : m_vecMaterial)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
		{
			Safe_Release(iter.pTextures[i]);
		}
	}
	m_vecMaterial.clear();

	for (auto& iter : m_vecMesh)
	{
		Safe_Release(iter);
	}
	m_vecMesh.clear();

	m_Importer.FreeScene();
}
