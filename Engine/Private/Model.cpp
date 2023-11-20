#include <filesystem>
#include <fstream>
#include <commdlg.h>

#include "Model.h"

#include "GameInstance.h"

#include "Mesh.h"
#include "Texture.h"
#include "Bone.h"
#include "Animation.h"

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
	, m_iNumAnimation(rhs.m_iNumAnimation)
	, m_matPivot(rhs.m_matPivot)
{
	// �ִϸ��̼ǰ� ���� ���� ���� �Ͽ� ������.
	for (auto& iter : rhs.m_vecAnimation)
		m_vecAnimation.push_back(iter->Clone());
	
	for (auto& iter : rhs.m_vecBone)
		m_vecBone.push_back(iter->Clone());

	for (auto& iter : m_vecMaterial)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
		{
			Safe_AddRef(iter.pTextures[i]);
		}
	}

	for (auto& iter : m_vecMesh)
	{
		Safe_AddRef(iter);
	}
}

HRESULT CModel::Initialize_ProtoType(const char* _strModleFilePath, _fmatrix _matPivot, MODEL_TYPE _eType)
{
	// fbx������ �о� �����͸� m_pAIScene���� ����.
	/*  aiProcess_PreTransformVertices 
		�޽��� �̸��� ���� �̸��� ���� �ִ��� �˻��Ѵ�.
		�ִٸ� �ε� �� �� �� ���� ����� ������ �޽��� ��� ������ �����ش�.
		���� ���� �ҷ��� ������ ����� ������ �ִϸ��̼��� �����ϴ� ���� ���� �ε��� ��
		�̹� ������ ���� ���·� �ε��ϰ� �ִϸ��̼� ������ ���� �ε��Ѵ�.*/

	// aiProcess_ConvertToLeftHanded : �޼� ��ǥ��� �˾Ƽ� ��ȯ�Ͽ� ��������(FBX SDK�� ���� ����)
	// aiProcessPreset_TargetRealtime_Fast : �ɼ��� ������ �����ν� ����Ƽ�� ���̰� �ִ�.
	
	// �⺻���� �÷���.
	//_uint iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	//// �Ű��� ���� Ÿ���� NonAnim�� �� aiProcess_PreTransformVertices�� ���ε� ���ش�.
	//if (_eType == TYPE_NONANIM)
	//	iFlag = iFlag | aiProcess_PreTransformVertices;
	//
	//m_pAIScene = m_Importer.ReadFile(_strModleFilePath, iFlag);
	//if (m_pAIScene == nullptr)
	//	return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pAIScene = pGameInstance->Binary_InFile(_strModleFilePath, _eType);

	Safe_Release(pGameInstance);

	// matrix Ÿ���� ������ float4�� ��ȯ
	XMStoreFloat4x4(&m_matPivot, _matPivot);

	// DirectX�� �׷��� �� �ְ� �����͸� �����ϴ� �κ�
	
	/* �� �ε�(�ֻ��� �θ� ��带 �Ű��� ������.) */
	//if (FAILED(Ready_Bone(&m_pAIScene->mVecNode[0], -1)))
	//if (FAILED(Ready_Bone(m_pAIScene->mVecNode[0], -1)))
	if (FAILED(Ready_Bone()))
		return E_FAIL;

	/* ���� �� ���� ���� ������ �ε�. */
	if (FAILED(Ready_Mesh(_eType)))
		return E_FAIL;

	/* ���� �ȼ����� ���ǵ� ���� ������ �������� �ؽ��ĸ� ����. */
	if (FAILED(Ready_Material(_strModleFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;


	return S_OK;
}

HRESULT CModel::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Bind_BondMatrices(CShader* _pShader, _uint _iMeshIndex, const char* _strConstantName)
{
	return m_vecMesh[_iMeshIndex]->Bind_BondMatrices(_pShader, m_vecBone, _strConstantName, XMLoadFloat4x4(&m_matPivot));
}

HRESULT CModel::Bind_MaterialTexture(CShader* _pShader, const char* _pConstantName, _uint _iMeshIndex, aiTextureType _eType)
{
	if (_iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint iMaterialIndex = m_vecMesh[_iMeshIndex]->Get_MaterialIndex();
	if (iMaterialIndex >= m_iNumMaterails)
		return E_FAIL;

	if (m_vecMaterial[iMaterialIndex].pTextures[_eType] == nullptr)
		return S_OK;

	return m_vecMaterial[iMaterialIndex].pTextures[_eType]->Bind_ShaderResource(_pShader, _pConstantName, 0);
}

HRESULT CModel::Render(_uint _iMeshIndex)
{
	// �޽ú��� ����ϴ� ���� ������ �ٸ� ���� �ֱ⿡ ���� ����� ���
	//for (auto& iter : m_vecMesh)
	//{
	//	if (iter != nullptr)
	//	{
	//		iter->Render();
	//	}
	//}

	m_vecMesh[_iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::SwapDesc_Armor(CModel* _pModel)
{
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

	for (auto& iter : m_vecBone)
	{
		Safe_Release(iter);
	}
	m_vecBone.clear();


	// safe_addref
	// safe_release�� ������ ����°� �ƴ϶� ī��Ʈ�� 0�̸� �����.

	m_vecBone = _pModel->m_vecBone;
	m_iNumMeshes = _pModel->m_iNumMeshes;
	m_vecMesh = _pModel->m_vecMesh;
	m_iNumMaterails = _pModel->m_iNumMaterails;
	m_vecMaterial = _pModel->m_vecMaterial;

	//Ready_Bone();
	//Ready_Mesh(TYPE_ANIM);
	// Ready_Material();

	return S_OK;
}

HRESULT CModel::Ready_Bone()
{
	for (size_t i = 0; i < m_pAIScene->mVecNode.size(); ++i)
	{
		CBone* pBone = CBone::Create(&m_pAIScene->mVecNode[i], m_pAIScene->mVecNode[i].mParentIndex);
		m_vecBone.push_back(pBone);
	}

	// �� ���� (�ֻ��� �θ� ��尡 ���� ó���� ������ ������ �Ű��� -1�� ����)
	//CBone* pBone = CBone::Create(_pRootNode, _iParentBoneIndex);
	//if (pBone == nullptr)
	//	return E_FAIL;
	//

	// 	m_vecBone.push_back(pBone);

	// ���� �θ� �ε��� ����.
	//_int iParentIndex = m_vecBone.size() - 1;

	// �ڽ� ��� �� ��ŭ ��� �Լ� �����Ͽ� �� ����.
	// �� �̻� ���� �� �ڽ��� ���� �� ���� ����ؼ� ��͸� ���� �� ���̴�.
	//for (size_t i = 0; i < _pRootNode->mNumChildren; ++i)
	//{
	//	// ��͸� ���� i��° �ڽ� ��带 �����Ѵ�.
	//	// �θ��� �ε����� �ݺ��� ������ ������ �� �ε����� �� ���̴�.
	//	Ready_Bone(&_pRootNode->mChildren[i], iParentIndex);
	//}

	return S_OK;
}
HRESULT CModel::Ready_Mesh(MODEL_TYPE _eType)
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	m_vecMesh.reserve(m_iNumMeshes);

	for (size_t i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, this, &m_pAIScene->mMeshs[i], XMLoadFloat4x4(&m_matPivot), _eType, &m_vecBone);
		if (pMesh == nullptr)
			return E_FAIL;

		m_vecMesh.push_back(pMesh);
	}

	return S_OK;
}
HRESULT CModel::Ready_Material(const char* _pModelFilePath)
{
	// ���� ������ �����
	m_iNumMaterails = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterails; ++i)
	{
		// aiMaterial* pMaterials = m_pAIScene->mMaterials[i];
		// aiMaterial* pMaterials = new aiMaterial;
		// �ʱ�ȭ
		MESH_MATERIAL MeshMaterial;
		ZeroMemory(&MeshMaterial, sizeof(MeshMaterial));

		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			//aiString strTexturePath;
			//
			//// �ش� ���� ������ ������ ��������
			//if (FAILED(pMaterials->GetTexture(aiTextureType(j), 0, &strTexturePath)))
			//	continue;
			//
			//// ����̺�, ��� ����
			//char			szDrive[MAX_PATH] = "";
			//char			szDirectory[MAX_PATH] = "";
			//_splitpath_s(_pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
			//
			//// ���ϸ�.Ȯ���� ����
			//char			szFileName[MAX_PATH] = "";
			//char			szExt[MAX_PATH] = "";
			//_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
			//
			//// ����̺� ���� �� �ڿ� ���, ���ϸ�, Ȯ���ڸ� �̾� ���δ�.
			//char			szTmp[MAX_PATH] = "";
			//strcpy_s(szTmp, szDrive);
			//strcat_s(szTmp, szDirectory);
			//strcat_s(szTmp, szFileName);
			//strcat_s(szTmp, szExt);

			if (m_pAIScene->mMaterials[i].mBIsReturn[j] != 1)
				continue;

			// ���ڿ� Ÿ�� ��ȯ �ʿ�.
			_tchar			szFullPath[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, m_pAIScene->mMaterials[i].mStrTextureFilePath[j].c_str(), 
				strlen(m_pAIScene->mMaterials[i].mStrTextureFilePath[j].c_str()), szFullPath, MAX_PATH);

			// ������ ������ ��η� �ؽ��ĸ� �ҷ��´�.
			MeshMaterial.pTextures[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath);
			if (nullptr == MeshMaterial.pTextures[j])
				return E_FAIL;
		}

		// �ؽ��ĵ��� �� ������ ����ü push.
		m_vecMaterial.push_back(MeshMaterial);
	}

	return S_OK;
}
HRESULT CModel::Ready_Animation()
{
	m_iNumAnimation = m_pAIScene->m_iNumAnimation;

	for (size_t i = 0; i < m_iNumAnimation; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(this, &m_pAIScene->mAnimations[i]);
		if (pAnimation == nullptr)
			return E_FAIL;

		m_vecAnimation.push_back(pAnimation);
	}

	return S_OK;
}

_int CModel::Get_BoneIndex(const char* _strBoneName) const
{
	for (size_t iBoneIndex = 0; iBoneIndex < m_vecBone.size(); ++iBoneIndex)
	{
		if (false == strcmp(m_vecBone[iBoneIndex]->Get_BoneName(), _strBoneName))
			return (_int)iBoneIndex;
	}

	return -1;
}
CBone* CModel::Get_BonePtr(const char* _strBoneName) const
{
	//auto	iter = find_if(m_vecBone.begin(), m_vecBone.end(), [&](CBone* pBone)
	//	{
	//		if (false == strcmp(pBone->Get_BoneName(), _strBoneName))
	//			return true;
	//		return false;
	//	});
	//
	//return *iter;

	for (size_t iBoneIndex = 0; iBoneIndex < m_vecBone.size(); ++iBoneIndex)
	{
		if (false == strcmp(m_vecBone[iBoneIndex]->Get_BoneName(), _strBoneName))
			return m_vecBone[iBoneIndex];
	}

	return nullptr;
}

HRESULT CModel::SetUp_Animation(_bool _bIsLoop, string _strAnimationName, _uint _iChangeIndex)
{
	m_bIsFindAnimation = false;
	_int iAnimationIndex = -1;

	/* ���� ó�� */
	/* �̹� ���� ���� �ִϸ��̼����� */
	// if (!strcmp(m_vecAnimation[m_iCurAnimationIndex]->Get_AnimationName(), _strAnimationName.c_str()))
	// {
	// 	return S_OK;
	// }

	/* �����ϴ� �ִϸ��̼����� */
	for (_uint i = 0; i < m_iNumAnimation; ++i)
	{
		if (!strcmp(m_vecAnimation[i]->Get_AnimationName(), _strAnimationName.c_str()))
		{
			iAnimationIndex = i;
			m_bIsFindAnimation = true;
			break;
		}
	}

	if (!m_bIsFindAnimation)
		return S_OK;


	if (m_bIsChanging)
	{
		m_iNextAnimationIndex = iAnimationIndex;
		m_bIsChanging = false;
	}

	// ���� �ϱ� �� ����
	if (m_iCurAnimationIndex != iAnimationIndex)
	{
		m_iNextAnimationIndex = iAnimationIndex;
		m_vecAnimation[m_iNextAnimationIndex]->Set_Loop(_bIsLoop);
		m_iChangeIndex = _iChangeIndex;
		m_vecAnimation[m_iCurAnimationIndex]->Ready_ChangeAnimation(m_iChangeIndex);
		m_bIsChanging = true;
	}
	//else
	//{
	//	m_vecAnimation[m_iCurAnimationIndex]->ReSet();
	//	//m_iCurAnimationIndex = iAnimationIndex;
	//	m_vecAnimation[m_iCurAnimationIndex]->Set_Loop(_bIsLoop);
	//}

	return S_OK;
}
HRESULT CModel::Play_Animation(_float _fTimeDelta)
{
	/* Non Anim ���̸� �ٷ� return */
	if (0 == m_iNumAnimation)
		return S_OK;

	/* ������ TransformationMatrix�� �ִϸ��̼� ���¿� �µ��� �ٲ��ش�.
	   (Animation -> Channel -> �ð��� �´� KeyFrame)*/

	   /* �� �ִϸ��̼ǿ� ���Ǵ� �θ� ������� �ڱ� �ڽ��� ���� �����ϰ� */
	if (!m_bIsChanging)
	{
		m_vecAnimation[m_iCurAnimationIndex]->Update_TransformationMatrix(m_vecBone, _fTimeDelta);
	}
	else if (m_bIsChanging)
	{
		if (m_vecAnimation[m_iCurAnimationIndex]->Change_TransformationMatrix(m_vecBone, m_vecAnimation[m_iNextAnimationIndex]->Get_Channel(), _fTimeDelta))
		{
			m_bIsChanging = false;
			m_vecAnimation[m_iCurAnimationIndex]->ReSet();
			m_iCurAnimationIndex = m_iNextAnimationIndex;
			m_vecAnimation[m_iNextAnimationIndex]->Reset_TrackPosition(m_iChangeIndex);
		}
	}

	/* ��� ������ CombinedTransformationMatrix�� �����Ѵ�. */
	for (size_t i = 0; i < m_vecBone.size(); ++i)
	{
		m_vecBone[i]->Update_CombinedTransformationMatrix(m_vecBone);
	}

	return S_OK;
}
string CModel::Get_CurAnimationName()
{
	return m_vecAnimation[m_iCurAnimationIndex]->Get_AnimationName();
}
_bool CModel::Get_IsAnimationFin()
{
	return m_vecAnimation[m_iCurAnimationIndex]->Get_Finish();
}
_bool CModel::Get_CurAnimationIsLoop()
{
	return m_vecAnimation[m_iCurAnimationIndex]->Get_CurAnimationIsLoop();
}
_uint CModel::Get_CurFrameIndex()
{
	_uint iIdx = 0;

	vector<_uint> vecCurKeyFrame = m_vecAnimation[m_iCurAnimationIndex]->Get_CurKeyFrame();

	for (_uint i = 0; i < vecCurKeyFrame.size(); ++i)
	{
		if (iIdx < vecCurKeyFrame[i])
			iIdx = vecCurKeyFrame[i];
	}

	return iIdx;
}

void CModel::Update_VI(const _fmatrix& _matPivot)
{
	for (size_t i = 0; i < m_vecMesh.size(); ++i)
	{
		m_vecMesh[i]->Update_VI(_matPivot);
	}
}

CModel* CModel::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const char* _strModleFilePath, _fmatrix _matPivot, MODEL_TYPE _eType)
{
	CModel* pInstance = new CModel(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType(_strModleFilePath, _matPivot, _eType)))
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

	for (auto& iter : m_vecAnimation)
	{
		Safe_Release(iter);
	}
	m_vecAnimation.clear();

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

	for (auto& iter : m_vecBone)
	{
		Safe_Release(iter);
	}
	m_vecBone.clear();

	Safe_Release(m_pAIScene);

	// m_Importer.FreeScene();
}
