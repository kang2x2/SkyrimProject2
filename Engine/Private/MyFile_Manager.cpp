#include "MyFile_Manager.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"
#include "Transform.h"
#include "Model.h"

#include "Bin_AIScene.h"

IMPLEMENT_SINGLETON(CMyFile_Manager)

CMyFile_Manager::CMyFile_Manager()
{
}

HRESULT CMyFile_Manager::Object_FileSave(ofstream& _outFile, _uint _iLevelIndex) 
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	map<const wstring, CLayer*>* pLayerMapAry = pGameInstance->Get_CloneObjectMapAry(_iLevelIndex);

	for (auto Layer = pLayerMapAry->begin(); Layer != pLayerMapAry->end(); ++Layer)
	{
		// Layer 내부의 ObjList를 가져옴.
		list<CGameObject*> objList = Layer->second->Get_ObjList();
		for (auto obj : objList)
		{
			if (!obj->Get_IsHasMesh())
				continue;

			CTransform* pTransform = dynamic_cast<CTransform*>(obj->Get_Component(TEXT("Com_Transform")));

			FILE_INOUTDESC tOutFileDesc;
			tOutFileDesc.m_strLayerTag = obj->Get_ObjFileDesc().m_strLayerTag;
			tOutFileDesc.m_strProtoObjTag = obj->Get_ObjFileDesc().m_strProtoObjTag;
			tOutFileDesc.m_strProtoModelComTag = obj->Get_ObjFileDesc().m_strProtoModelComTag;
			tOutFileDesc.m_matWorld = pTransform->Get_WorldMatrix();

			size_t iStrLength = tOutFileDesc.m_strLayerTag.length();
			_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
			_outFile.write(reinterpret_cast<const char*>(tOutFileDesc.m_strLayerTag.c_str()), iStrLength * sizeof(wchar_t));

			iStrLength = tOutFileDesc.m_strProtoObjTag.length();
			_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
			_outFile.write(reinterpret_cast<const char*>(tOutFileDesc.m_strProtoObjTag.c_str()), iStrLength * sizeof(wchar_t));

			iStrLength = tOutFileDesc.m_strProtoModelComTag.length();
			_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
			_outFile.write(reinterpret_cast<const char*>(tOutFileDesc.m_strProtoModelComTag.c_str()), iStrLength * sizeof(wchar_t));

			_outFile.write(reinterpret_cast<const char*>(&tOutFileDesc.m_matWorld), sizeof(_matrix));
		}
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CMyFile_Manager::Object_FileLoad(ifstream& _inFile, _uint _iLevelIndex)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	size_t		   iStrLength = 0;

	// while (!_inFile.eof()) : 다 읽고 나서도 쓰레기를 한 번 더 읽어서 위험. 아래와 같이 하자.
	while (_inFile.read(reinterpret_cast<char*>(&iStrLength), sizeof(size_t)))
	{
		FILE_INOUTDESC tInFileDesc;

		tInFileDesc.m_strLayerTag.resize(iStrLength);
		_inFile.read(reinterpret_cast<char*>(&tInFileDesc.m_strLayerTag[0]), iStrLength * sizeof(wchar_t));

		_inFile.read(reinterpret_cast<char*>(&iStrLength), sizeof(size_t));
		tInFileDesc.m_strProtoObjTag.resize(iStrLength);
		_inFile.read(reinterpret_cast<char*>(&tInFileDesc.m_strProtoObjTag[0]), iStrLength * sizeof(wchar_t));

		_inFile.read(reinterpret_cast<char*>(&iStrLength), sizeof(size_t));
		tInFileDesc.m_strProtoModelComTag.resize(iStrLength);
		_inFile.read(reinterpret_cast<char*>(&tInFileDesc.m_strProtoModelComTag[0]), iStrLength * sizeof(wchar_t));

		_inFile.read(reinterpret_cast<char*>(&tInFileDesc.m_matWorld), sizeof(_matrix));

		// clone object
		if (FAILED(pGameInstance->Add_CloneObject(_iLevelIndex, tInFileDesc.m_strLayerTag,
			tInFileDesc.m_strProtoObjTag, tInFileDesc.m_strProtoModelComTag, &tInFileDesc.m_matWorld)))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CMyFile_Manager::Binary_OutFile(ofstream& _outFile, const CString& _strFilePath, CModel::MODEL_TYPE _eType)
{
	// wstring -> string(const char*)
	const wchar_t* wStrModelPath = _strFilePath;

	// wchar_t 배열을 UTF-8로 변환
	int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wStrModelPath, -1, nullptr, 0, nullptr, nullptr);
	string str(utf8Length, '\0');
	WideCharToMultiByte(CP_UTF8, 0, wStrModelPath, -1, &str[0], utf8Length, nullptr, nullptr);

	// 읽어들인 파일 복사.
	Binary_NonAnimCopy(str.c_str(), _eType);

	if (m_pBin_AIScene == nullptr)
		return E_FAIL;
			
	/* 뼈 */
	_outFile.write(reinterpret_cast<const char*>(m_pBin_AIScene->mRootNode), sizeof(aiNode));
	_outFile.write(reinterpret_cast<const char*>(&m_pBin_AIScene->mRootNode->mNumChildren), sizeof(unsigned int));
	
	for (size_t i = 0; i < m_pBin_AIScene->mRootNode->mNumChildren; ++i)
	{
		_outFile.write(reinterpret_cast<const char*>(m_pBin_AIScene->mRootNode->mChildren[i]), sizeof(aiNode));

		/* 문자열 길이 */
		size_t iStrLength = 0;
		aiString aiStr;
		aiStr.Set(m_pBin_AIScene->mRootNode->mChildren[i]->mName.data);
		
		while (aiStr.C_Str()[iStrLength] != '\0')
			iStrLength++;
		
		_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
		_outFile.write(reinterpret_cast<const char*>(m_pBin_AIScene->mRootNode->mChildren[i]->mName.data[0]), iStrLength * sizeof(wchar_t));
		// 
		// _outFile.write(reinterpret_cast<const char*>(m_pBin_AIScene->mRootNode->mChildren[i]->mName.data), 1024);
		_outFile.write(reinterpret_cast<const char*>(&m_pBin_AIScene->mRootNode->mChildren[i]->mTransformation), sizeof(aiMatrix4x4));
	}

	/* 메시 */
	_outFile.write(reinterpret_cast<const char*>(&m_pBin_AIScene->mNumMeshes), sizeof(unsigned int));
	for (size_t i = 0; i < m_pBin_AIScene->mNumMeshes; ++i)
	{
		_outFile.write(reinterpret_cast<const char*>(m_pBin_AIScene->mMeshes[i]), sizeof(aiMesh));

		_outFile.write(reinterpret_cast<const char*>(&m_pBin_AIScene->mMeshes[i]->mMaterialIndex), sizeof(unsigned int));
		_outFile.write(reinterpret_cast<const char*>(&m_pBin_AIScene->mMeshes[i]->mNumVertices), sizeof(unsigned int));
		_outFile.write(reinterpret_cast<const char*>(&m_pBin_AIScene->mMeshes[i]->mNumFaces), sizeof(unsigned int));
		
		_outFile.write(reinterpret_cast<const char*>(&m_pBin_AIScene->mMeshes[i]->mVertices), sizeof(aiVector3D));
		_outFile.write(reinterpret_cast<const char*>(&m_pBin_AIScene->mMeshes[i]->mNormals), sizeof(aiVector3D));
		_outFile.write(reinterpret_cast<const char*>(&m_pBin_AIScene->mMeshes[i]->mTextureCoords), sizeof(aiVector3D));
		_outFile.write(reinterpret_cast<const char*>(&m_pBin_AIScene->mMeshes[i]->mTangents), sizeof(aiVector3D));
	}

	/* 마테리얼 */
	_outFile.write(reinterpret_cast<const char*>(&m_pBin_AIScene->mNumMaterials), sizeof(unsigned int));
	for (size_t i = 0; i < m_pBin_AIScene->mNumMaterials; ++i)
	{
		_outFile.write(reinterpret_cast<const char*>(m_pBin_AIScene->mMaterials[i]), sizeof(aiMaterial));
	}

	/* 애니메이션 */
	_outFile.write(reinterpret_cast<const char*>(&m_pBin_AIScene->mNumAnimations), sizeof(unsigned int));
	for (size_t i = 0; i < m_pBin_AIScene->mNumAnimations; ++i)
	{
		_outFile.write(reinterpret_cast<const char*>(m_pBin_AIScene->mAnimations[i]), sizeof(aiAnimation));
		/* 문자열 길이 */
		size_t iStrLength = 0;
		aiString aiStr;
		aiStr.Set(m_pBin_AIScene->mRootNode->mChildren[i]->mName.data);

		while (aiStr.C_Str()[iStrLength] != '\0')
			iStrLength++;

		_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
		_outFile.write(reinterpret_cast<const char*>(m_pBin_AIScene->mAnimations[i]->mName.data[0]), iStrLength * sizeof(wchar_t));
		_outFile.write(reinterpret_cast<const char*>(&m_pBin_AIScene->mAnimations[i]->mDuration), sizeof(double));
		
		/* 채널 */
		_outFile.write(reinterpret_cast<const char*>(&m_pBin_AIScene->mAnimations[i]->mNumChannels), sizeof(unsigned int));
		for (size_t j = 0; j < m_pBin_AIScene->mAnimations[i]->mNumChannels; ++j)
		{
			_outFile.write(reinterpret_cast<const char*>(m_pBin_AIScene->mAnimations[i]->mChannels[j]), sizeof(aiNodeAnim));
			/* 문자열 길이 */
			size_t iStrLength = 0;
			aiString aiStr;
			aiStr.Set(m_pBin_AIScene->mAnimations[i]->mChannels[j]->mNodeName.data);
			
			while (aiStr.C_Str()[iStrLength] != '\0')
				iStrLength++;
			
			_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
			_outFile.write(reinterpret_cast<const char*>(m_pBin_AIScene->mAnimations[i]->mChannels[j]->mNodeName.data[0]), iStrLength * sizeof(wchar_t));
			// 
			_outFile.write(reinterpret_cast<const char*>(&m_pBin_AIScene->mAnimations[i]->mChannels[j]->mNumScalingKeys), sizeof(unsigned int));
			_outFile.write(reinterpret_cast<const char*>(&m_pBin_AIScene->mAnimations[i]->mChannels[j]->mNumRotationKeys), sizeof(unsigned int));
			_outFile.write(reinterpret_cast<const char*>(&m_pBin_AIScene->mAnimations[i]->mChannels[j]->mNumPositionKeys), sizeof(unsigned int));
		}
	}

	Safe_Release(m_pBin_AIScene);

	return S_OK;
}

HRESULT CMyFile_Manager::Binary_NonAnimCopy(const char* _strFilePath, CModel::MODEL_TYPE _eType)
{
	_uint iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	// 매개로 숫자가 0이면 NonAnim.
	if (_eType == 0)
		iFlag = iFlag | aiProcess_PreTransformVertices;

	// Read
	m_pAIScene = m_Importer.ReadFile(_strFilePath, iFlag);
	if (m_pAIScene == nullptr)
		return E_FAIL;

	// 바이너리화를 위한 담기.
	m_pBin_AIScene = CBin_AIScene::Create();

	/* 뼈 */
	m_pBin_AIScene->mRootNode = m_pAIScene->mRootNode;
	m_pBin_AIScene->mRootNode->mNumChildren = m_pAIScene->mRootNode->mNumChildren;
	m_pBin_AIScene->mRootNode->mChildren = m_pAIScene->mRootNode->mChildren;

	for (size_t i = 0; i < m_pBin_AIScene->mRootNode->mNumChildren; ++i)
	{
		m_pBin_AIScene->mRootNode->mChildren[i]->mTransformation = m_pAIScene->mRootNode->mChildren[i]->mTransformation;
		m_pBin_AIScene->mRootNode->mChildren[i]->mName = m_pAIScene->mRootNode->mChildren[i]->mName;
	}

	/* 메시 */
	m_pBin_AIScene->mMeshes = m_pAIScene->mMeshes;
	m_pBin_AIScene->mNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_pBin_AIScene->mNumMeshes; ++i)
	{
		m_pBin_AIScene->mMeshes[i]->mMaterialIndex = m_pAIScene->mMeshes[i]->mMaterialIndex;
		m_pBin_AIScene->mMeshes[i]->mNumVertices = m_pAIScene->mMeshes[i]->mNumVertices;
		m_pBin_AIScene->mMeshes[i]->mNumFaces = m_pAIScene->mMeshes[i]->mNumFaces;

		m_pBin_AIScene->mMeshes[i]->mVertices = m_pAIScene->mMeshes[i]->mVertices;
		m_pBin_AIScene->mMeshes[i]->mNormals = m_pAIScene->mMeshes[i]->mNormals;
		*m_pBin_AIScene->mMeshes[i]->mTextureCoords = *m_pAIScene->mMeshes[i]->mTextureCoords;
		m_pBin_AIScene->mMeshes[i]->mTangents = m_pAIScene->mMeshes[i]->mTangents;
	}

	/* 마테리얼 */
	m_pBin_AIScene->mMaterials = m_pAIScene->mMaterials;
	m_pBin_AIScene->mNumMaterials = m_pAIScene->mNumMaterials;

	/* 애니메이션 */
	m_pBin_AIScene->mAnimations = m_pAIScene->mAnimations;
	m_pBin_AIScene->mNumAnimations = m_pAIScene->mNumAnimations;
	
	for (size_t i = 0; i < m_pBin_AIScene->mNumAnimations; ++i)
	{
		m_pBin_AIScene->mAnimations[i]->mName = m_pAIScene->mAnimations[i]->mName;
		m_pBin_AIScene->mAnimations[i]->mDuration = m_pAIScene->mAnimations[i]->mDuration;
		
		/* 채널 */
		m_pBin_AIScene->mAnimations[i]->mChannels = m_pAIScene->mAnimations[i]->mChannels;
		m_pBin_AIScene->mAnimations[i]->mNumChannels = m_pAIScene->mAnimations[i]->mNumChannels;
		for (size_t j = 0; j < m_pBin_AIScene->mAnimations[i]->mNumChannels; ++j)
		{
			m_pBin_AIScene->mAnimations[i]->mChannels[j]->mNodeName = m_pAIScene->mAnimations[i]->mChannels[j]->mNodeName;
			m_pBin_AIScene->mAnimations[i]->mChannels[j]->mNumScalingKeys = m_pAIScene->mAnimations[i]->mChannels[j]->mNumScalingKeys;
			m_pBin_AIScene->mAnimations[i]->mChannels[j]->mNumRotationKeys = m_pAIScene->mAnimations[i]->mChannels[j]->mNumRotationKeys;
			m_pBin_AIScene->mAnimations[i]->mChannels[j]->mNumPositionKeys = m_pAIScene->mAnimations[i]->mChannels[j]->mNumPositionKeys;
		}
	}

	return S_OK;
}

CBin_AIScene* CMyFile_Manager::Binary_InFile(ifstream& _inFile, const char* _strFilePath)
{
	m_pBin_AIScene = CBin_AIScene::Create();
	size_t iStrLength = 0;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	// while (!_inFile.eof()) : 다 읽고 나서도 쓰레기를 한 번 더 읽어서 위험. 아래와 같이 하자.
	while (_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mRootNode), sizeof(aiNode*)))
	{
		/* 뼈 */
		_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mRootNode->mNumChildren), sizeof(unsigned int));
		_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mRootNode->mChildren), sizeof(aiNode**));
		for (size_t i = 0; i < m_pBin_AIScene->mRootNode->mNumChildren; ++i)
		{
			// _inFile.read(reinterpret_cast<char*>(&iStrLength), sizeof(size_t));
			// _inFile.read(reinterpret_cast<char*>(m_pBin_AIScene->mRootNode->mChildren[i]->mName.data), iStrLength * sizeof(wchar_t));
			_inFile.read(reinterpret_cast<char*>(m_pBin_AIScene->mRootNode->mChildren[i]->mName.data), 1024);
			_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mRootNode->mChildren[i]->mTransformation), sizeof(aiMatrix4x4));
		}

		/* 메시 */
		_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mMeshes), sizeof(aiMesh**));
		_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mNumMeshes), sizeof(unsigned int));

		for (size_t i = 0; i < m_pBin_AIScene->mNumMeshes; ++i)
		{
			_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mMeshes[i]->mMaterialIndex), sizeof(unsigned int));
			_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mMeshes[i]->mNumVertices), sizeof(unsigned int));
			_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mMeshes[i]->mNumFaces), sizeof(unsigned int));
			
			_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mMeshes[i]->mVertices), sizeof(aiVector3D*));
			_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mMeshes[i]->mNormals), sizeof(aiVector3D*));
			_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mMeshes[i]->mTextureCoords), sizeof(aiVector3D*));
			_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mMeshes[i]->mTangents), sizeof(aiVector3D*));
		}

		/* 마테리얼 */
		_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mMaterials), sizeof(aiMaterial**));
		_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mNumMaterials), sizeof(unsigned int));

		/* 애니메이션 */
		_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mAnimations), sizeof(aiAnimation**));
		_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mNumAnimations), sizeof(unsigned int));
		for (size_t i = 0; i < m_pBin_AIScene->mNumAnimations; ++i)
		{
			//_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mAnimations[i]->mName.data), sizeof(aiString));
			_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mAnimations[i]->mName.data), 1024);
			_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mAnimations[i]->mDuration), sizeof(double));

			/* 채널 */
			_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mAnimations[i]->mChannels), sizeof(aiNodeAnim**));
			_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mAnimations[i]->mNumChannels), sizeof(unsigned int));
			for (size_t j = 0; j < m_pBin_AIScene->mAnimations[i]->mNumChannels; ++j)
			{
				_inFile.read(reinterpret_cast<char*>(m_pBin_AIScene->mAnimations[i]->mChannels[j]->mNodeName.data), 1024);
				// 
				_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mAnimations[i]->mChannels[j]->mNumScalingKeys), sizeof(unsigned int));
				_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mAnimations[i]->mChannels[j]->mNumRotationKeys), sizeof(unsigned int));
				_inFile.read(reinterpret_cast<char*>(&m_pBin_AIScene->mAnimations[i]->mChannels[j]->mNumPositionKeys), sizeof(unsigned int));
			}
		}

	}

	Safe_Release(pGameInstance);


	return m_pBin_AIScene;
}


void CMyFile_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pBin_AIScene);
}

