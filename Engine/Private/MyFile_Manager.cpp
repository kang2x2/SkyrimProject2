#include <fstream>

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

HRESULT CMyFile_Manager::Binary_OutFile(ofstream& _outFile, const char* _strFilePath, CModel::MODEL_TYPE _eType)
{
	// 기본적인 플래그.
	_uint iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	// 매개로 들어온 타입이 NonAnim일 때 aiProcess_PreTransformVertices도 바인딩 해준다.
	if (_eType == CModel::TYPE_NONANIM)
		iFlag = iFlag | aiProcess_PreTransformVertices;

	m_pAIScene = m_Importer.ReadFile(_strFilePath, iFlag);
	if (m_pAIScene == nullptr)
		return E_FAIL;

	m_pBinAiScene = CBin_AIScene::Create();

	if (FAILED(Write_FBXNode(_outFile)))
	{
		MSG_BOX("Fail Binary Write : Bone");
		return E_FAIL;
	}
	if (FAILED(Write_FBXMesh(_outFile, _eType)))
	{
		MSG_BOX("Fail Binary Write : Mesh");
		return E_FAIL;
	}
	if (FAILED(Write_FBXMaterial(_outFile)))
	{
		MSG_BOX("Fail Binary Write : Material");
		return E_FAIL;
	}
	if (FAILED(Write_FBXAnimation(_outFile)))
	{
		MSG_BOX("Fail Binary Write : Animation");
		return E_FAIL;
	}

	Safe_Release(m_pBinAiScene);

	return S_OK;
}

HRESULT CMyFile_Manager::Write_FBXNode(ofstream& _outFile)
{
	m_pBinAiScene->mRootNode.mNumChildren = m_pAIScene->mRootNode->mNumChildren;
	m_pBinAiScene->mRootNode.mName = m_pAIScene->mRootNode->mName;
	m_pBinAiScene->mRootNode.mTransformation = m_pAIScene->mRootNode->mTransformation;
	
	/* Root Node */
	_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mRootNode.mNumChildren), sizeof(unsigned int));
	size_t iStrLength = m_pBinAiScene->mRootNode.mName.length;
	_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
	_outFile.write(reinterpret_cast<const char*>(m_pBinAiScene->mRootNode.mName.data), iStrLength * sizeof(char));
	_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mRootNode.mTransformation), sizeof(aiMatrix4x4));

	if (m_pBinAiScene->mRootNode.mNumChildren != 0)
	{
		m_pBinAiScene->mRootNode.mChildren = new CBin_AIScene::DESC_NODE[m_pBinAiScene->mRootNode.mNumChildren];

		m_pBinAiScene->mRootNode.mChildren[0].mName = m_pAIScene->mRootNode->mChildren[0]->mName;
		m_pBinAiScene->mRootNode.mChildren[0].mNumChildren = m_pAIScene->mRootNode->mChildren[0]->mNumChildren;
		m_pBinAiScene->mRootNode.mChildren[0].mTransformation = m_pAIScene->mRootNode->mChildren[0]->mTransformation;
		
		_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mRootNode->mChildren[0]->mNumChildren), sizeof(unsigned int));
		size_t iStrLength = m_pBinAiScene->mRootNode.mName.length;
		_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
		_outFile.write(reinterpret_cast<const char*>(m_pAIScene->mRootNode->mChildren[0]->mName.data), iStrLength * sizeof(char));
		_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mRootNode->mChildren[0]->mTransformation), sizeof(aiMatrix4x4));

		Write_FBXChildrenNode(_outFile, m_pAIScene->mRootNode->mChildren[0]);

		delete[] m_pBinAiScene->mRootNode.mChildren;
	}

	return S_OK;
}

HRESULT CMyFile_Manager::Write_FBXChildrenNode(ofstream& _outFile, aiNode* _node)
{
	CBin_AIScene::DESC_NODE tempNode;

	tempNode.mNumChildren = _node->mNumChildren;
	tempNode.mName = _node->mName;
	tempNode.mTransformation = _node->mTransformation;

	_outFile.write(reinterpret_cast<const char*>(&tempNode.mNumChildren), sizeof(unsigned int));
	size_t iStrLength = m_pBinAiScene->mRootNode.mName.length;
	_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
	_outFile.write(reinterpret_cast<const char*>(tempNode.mName.data), iStrLength * sizeof(char));
	_outFile.write(reinterpret_cast<const char*>(&tempNode.mTransformation), sizeof(aiMatrix4x4));

	for (size_t i = 0; i < tempNode.mNumChildren; ++i)
	{
		Write_FBXChildrenNode(_outFile, m_pAIScene->mRootNode->mChildren[i]);
	}

	return S_OK;
}

HRESULT CMyFile_Manager::Write_FBXMesh(ofstream& _outFile, CModel::MODEL_TYPE _eType)
{
	m_pBinAiScene->mNumMeshes = m_pAIScene->mNumMeshes;
	size_t iStrLength = 0;

	m_pBinAiScene->mMeshs = new CBin_AIScene::DESC_MESH[m_pBinAiScene->mNumMeshes];

	/* 전체 메시 */
	for (size_t i = 0; i < m_pBinAiScene->mNumMeshes; ++i)
	{
		iStrLength = m_pBinAiScene->mMeshs[i].mName.length;
		_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
		_outFile.write(reinterpret_cast<const char*>(m_pBinAiScene->mMeshs[i].mName.data), iStrLength * sizeof(char));
	}

	/* 재질 */
	for (size_t i = 0; i < m_pBinAiScene->mNumMeshes; ++i)
	{
		_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mMeshs[i].mMaterialIndex), sizeof(unsigned int));
	}

	/* 메시 면 */
	for (size_t i = 0; i < m_pBinAiScene->mNumMeshes; ++i)
	{
		m_pBinAiScene->mMeshs[i].mNumFaces = m_pAIScene->mMeshes[i]->mNumFaces;
		_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mMeshs[i].mNumFaces), sizeof(unsigned int));

		m_pBinAiScene->mMeshs[i].mFaces = new CBin_AIScene::DESC_MESHFACE[m_pBinAiScene->mMeshs[i].mNumFaces];
		
		for (size_t j = 0; j < m_pBinAiScene->mMeshs[i].mNumFaces; ++j)
		{
			m_pBinAiScene->mMeshs[i].mFaces[j].mIndices[0] = m_pAIScene->mMeshes[i]->mFaces[j].mIndices[0];
			m_pBinAiScene->mMeshs[i].mFaces[j].mIndices[1] = m_pAIScene->mMeshes[i]->mFaces[j].mIndices[1];
			m_pBinAiScene->mMeshs[i].mFaces[j].mIndices[2] = m_pAIScene->mMeshes[i]->mFaces[j].mIndices[2];
			_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mMeshs[i].mFaces[j].mIndices), sizeof(unsigned int) * 3);
		}

		delete[] m_pBinAiScene->mMeshs[i].mFaces;
	}

	/* 버텍스 */
	for (size_t i = 0; i < m_pBinAiScene->mNumMeshes; ++i)
	{
		m_pBinAiScene->mMeshs[i].mNumVertices = m_pAIScene->mMeshes[i]->mNumVertices;
		_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mMeshs[i].mNumVertices), sizeof(unsigned int));
		
		m_pBinAiScene->mMeshs[i].mVertices = new aiVector3D[m_pBinAiScene->mMeshs[i].mNumVertices];
		m_pBinAiScene->mMeshs[i].mNormals = new aiVector3D[m_pBinAiScene->mMeshs[i].mNumVertices];
		m_pBinAiScene->mMeshs[i].mTextureCoords[0] = new aiVector3D[m_pBinAiScene->mMeshs[i].mNumVertices];
		m_pBinAiScene->mMeshs[i].mTangents = new aiVector3D[m_pBinAiScene->mMeshs[i].mNumVertices];
		
		for (size_t j = 0; j < m_pBinAiScene->mMeshs[i].mNumVertices; ++j)
		{
			m_pBinAiScene->mMeshs[i].mVertices[j] = m_pAIScene->mMeshes[i]->mVertices[j];
			m_pBinAiScene->mMeshs[i].mNormals[j] = m_pAIScene->mMeshes[i]->mNormals[j];
			m_pBinAiScene->mMeshs[i].mTextureCoords[0][j] = m_pAIScene->mMeshes[i]->mTextureCoords[0][j];
			m_pBinAiScene->mMeshs[i].mTangents[j] = m_pAIScene->mMeshes[i]->mTangents[j];
			
			_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mMeshs[i].mVertices[j]), sizeof(_float3));
			_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mMeshs[i].mNormals[j]), sizeof(_float3));
			_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mMeshs[i].mTextureCoords[0][j]), sizeof(_float2));
			_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mMeshs[i].mTangents[j]), sizeof(_float3));
		}

		delete[] m_pBinAiScene->mMeshs[i].mVertices;
		delete[] m_pBinAiScene->mMeshs[i].mNormals;
		delete[] m_pBinAiScene->mMeshs[i].mTextureCoords[0];
		delete[] m_pBinAiScene->mMeshs[i].mTangents;
	}

	/* 메시 본 */
	if (_eType == CModel::TYPE_ANIM)
	{
		for (size_t i = 0; i < m_pBinAiScene->mNumMeshes; ++i)
		{
			m_pBinAiScene->mMeshs[i].mBones = new CBin_AIScene::DESC_MESHBONE[m_pBinAiScene->mNumMeshes];

			_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mMeshs[i].mNumBones), sizeof(unsigned int));
			for (size_t j = 0; j < m_pBinAiScene->mMeshs[i].mNumBones; ++j)
			{
				m_pBinAiScene->mMeshs[i].mBones[j].mOffsetMatrix = m_pAIScene->mMeshes[i]->mBones[j]->mOffsetMatrix;
				_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mMeshs[i].mBones[j].mOffsetMatrix), sizeof(aiMatrix4x4));

				m_pBinAiScene->mMeshs[i].mBones[j].mName = m_pAIScene->mMeshes[i]->mBones[j]->mName;
				iStrLength = m_pBinAiScene->mMeshs[i].mBones[j].mName.length;
				_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
				_outFile.write(reinterpret_cast<const char*>(m_pBinAiScene->mMeshs[i].mBones[j].mName.data), iStrLength * sizeof(char));

				m_pBinAiScene->mMeshs[i].mBones[j].mNumWeights = m_pAIScene->mMeshes[i]->mBones[j]->mNumWeights;
				_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mMeshs[i].mBones[j].mNumWeights), sizeof(unsigned int));

				/* 메시 본 웨이트 */
				m_pBinAiScene->mMeshs[i].mBones[j].mWeights = new CBin_AIScene::DESC_MESHBONEWEIGHT[m_pBinAiScene->mMeshs[i].mBones[j].mNumWeights];
				for (size_t k = 0; k < m_pBinAiScene->mMeshs[i].mBones[j].mNumWeights; ++k)
				{
					m_pBinAiScene->mMeshs[i].mBones[j].mWeights[k].mVertexId = m_pBinAiScene->mMeshs[i].mBones[j].mWeights[k].mVertexId;
					m_pBinAiScene->mMeshs[i].mBones[j].mWeights[k].mWeight = m_pBinAiScene->mMeshs[i].mBones[j].mWeights[k].mWeight;

					_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mMeshs[i].mBones[j].mWeights[k].mVertexId), sizeof(unsigned int));
					_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mMeshs[i].mBones[j].mWeights[k].mWeight), sizeof(_float));
				}
				delete[] m_pBinAiScene->mMeshs[i].mBones[j].mWeights;
			}
			delete[] m_pBinAiScene->mMeshs[i].mBones;
		}
	}

	delete[] m_pBinAiScene->mMeshs;

	return S_OK;
}

HRESULT CMyFile_Manager::Write_FBXMaterial(ofstream& _outFile)
{
	//m_pBinAiScene->mNumMaterials = m_pAIScene->mNumMaterials;
	//
	//for (size_t i = 0; i < m_pBinAiScene->mNumMaterials; ++i)
	//{
	//	m_pBinAiScene->mMaterials[i] = m_pAIScene->mMaterials[i];
	//	_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mMaterials[i]), sizeof(aiMaterial));
	//}

	return S_OK;
}

HRESULT CMyFile_Manager::Write_FBXAnimation(ofstream& _outFile)
{
	m_pBinAiScene->m_iNumAnimation = m_pAIScene->mNumAnimations;

	m_pBinAiScene->mAnimations = new CBin_AIScene::DESC_ANIMATION[m_pBinAiScene->m_iNumAnimation];

	size_t iStrLength = 0;
	for (size_t i = 0; i < m_pBinAiScene->m_iNumAnimation; ++i)
	{
		m_pBinAiScene->mAnimations[i].mName = m_pAIScene->mAnimations[i]->mName;
		iStrLength = m_pBinAiScene->mAnimations[i].mName.length;
		_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
		_outFile.write(reinterpret_cast<const char*>(m_pBinAiScene->mAnimations[i].mName.data), iStrLength * sizeof(char));
		
		m_pBinAiScene->mAnimations[i].mDuration = (_float)m_pAIScene->mAnimations[i]->mDuration;
		m_pBinAiScene->mAnimations[i].mTicksPerSecond = (_float)m_pAIScene->mAnimations[i]->mTicksPerSecond;
		m_pBinAiScene->mAnimations[i].mNumChannels = m_pAIScene->mAnimations[i]->mNumChannels;
		
		_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mAnimations[i].mDuration), sizeof(_float));
		_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mAnimations[i].mTicksPerSecond), sizeof(_float));
		_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mAnimations[i].mNumChannels), sizeof(unsigned int));

		m_pBinAiScene->mAnimations[i].mChannels = new CBin_AIScene::DESC_ANIMATIONCHANNEL[m_pBinAiScene->mAnimations[i].mNumChannels];

		for (size_t j = 0; j < m_pBinAiScene->mAnimations[i].mNumChannels; ++j)
		{
			m_pBinAiScene->mAnimations[i].mChannels[j].mNodeName = m_pAIScene->mAnimations[i]->mChannels[j]->mNodeName;
			iStrLength = m_pBinAiScene->mAnimations[i].mChannels[j].mNodeName.length;
			_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
			_outFile.write(reinterpret_cast<const char*>(m_pBinAiScene->mAnimations[i].mChannels[j].mNodeName.data), iStrLength * sizeof(char));
			
			m_pBinAiScene->mAnimations[i].mChannels[j].mNumScalingKeys = m_pAIScene->mAnimations[i]->mChannels[j]->mNumScalingKeys;
			m_pBinAiScene->mAnimations[i].mChannels[j].mNumRotationKeys = m_pAIScene->mAnimations[i]->mChannels[j]->mNumRotationKeys;
			m_pBinAiScene->mAnimations[i].mChannels[j].mNumPositionKeys = m_pAIScene->mAnimations[i]->mChannels[j]->mNumPositionKeys;

			_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mAnimations[i].mChannels[j].mNumScalingKeys), sizeof(unsigned int));
			_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mAnimations[i].mChannels[j].mNumRotationKeys), sizeof(unsigned int));
			_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mAnimations[i].mChannels[j].mNumPositionKeys), sizeof(unsigned int));
		
			m_pBinAiScene->mAnimations[i].mChannels[j].mScalingKeys = new CBin_AIScene::DESC_CHANNELSCALEKEY[m_pBinAiScene->mAnimations[i].mChannels[j].mNumScalingKeys];
			m_pBinAiScene->mAnimations[i].mChannels[j].mRotationKeys = new CBin_AIScene::DESC_CHANNELROTKEY[m_pBinAiScene->mAnimations[i].mChannels[j].mNumRotationKeys];
			m_pBinAiScene->mAnimations[i].mChannels[j].mPositionKeys = new CBin_AIScene::DESC_CHANNELPOSKEY[m_pBinAiScene->mAnimations[i].mChannels[j].mNumPositionKeys];

			for (size_t k = 0; k < m_pBinAiScene->mAnimations[i].mChannels[j].mNumScalingKeys; ++k)
			{
				m_pBinAiScene->mAnimations[i].mChannels[j].mScalingKeys[k].mTime = (_float)m_pAIScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mTime;
				m_pBinAiScene->mAnimations[i].mChannels[j].mScalingKeys[k].mValue.x = m_pAIScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.x;
				m_pBinAiScene->mAnimations[i].mChannels[j].mScalingKeys[k].mValue.y = m_pAIScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.y;
				m_pBinAiScene->mAnimations[i].mChannels[j].mScalingKeys[k].mValue.z = m_pAIScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.z;
			
				_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mAnimations[i].mChannels[j].mScalingKeys[k].mTime), sizeof(_float));
				_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mAnimations[i].mChannels[j].mScalingKeys[k].mValue), sizeof(_float3));
			}
			for (size_t k = 0; k < m_pBinAiScene->mAnimations[i].mChannels[j].mNumRotationKeys; ++k)
			{
				m_pBinAiScene->mAnimations[i].mChannels[j].mRotationKeys[k].mTime = (_float)m_pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mTime;
				m_pBinAiScene->mAnimations[i].mChannels[j].mRotationKeys[k].mValue.x = m_pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.x;
				m_pBinAiScene->mAnimations[i].mChannels[j].mRotationKeys[k].mValue.y = m_pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.y;
				m_pBinAiScene->mAnimations[i].mChannels[j].mRotationKeys[k].mValue.z = m_pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.z;
				m_pBinAiScene->mAnimations[i].mChannels[j].mRotationKeys[k].mValue.w = m_pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.w;
			
				_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mAnimations[i].mChannels[j].mRotationKeys[k].mTime), sizeof(_float));
				_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mAnimations[i].mChannels[j].mRotationKeys[k].mValue), sizeof(_float4));
			}
			for (size_t k = 0; k < m_pBinAiScene->mAnimations[i].mChannels[j].mNumPositionKeys; ++k)
			{
				m_pBinAiScene->mAnimations[i].mChannels[j].mPositionKeys[k].mTime = (_float)m_pAIScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mTime;
				m_pBinAiScene->mAnimations[i].mChannels[j].mPositionKeys[k].mValue.x = m_pAIScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.x;
				m_pBinAiScene->mAnimations[i].mChannels[j].mPositionKeys[k].mValue.y = m_pAIScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.y;
				m_pBinAiScene->mAnimations[i].mChannels[j].mPositionKeys[k].mValue.z = m_pAIScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.z;
			
				_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mAnimations[i].mChannels[j].mPositionKeys[k].mTime), sizeof(_float));
				_outFile.write(reinterpret_cast<const char*>(&m_pBinAiScene->mAnimations[i].mChannels[j].mPositionKeys[k].mValue), sizeof(_float3));
			}

			delete[] m_pBinAiScene->mAnimations[i].mChannels[j].mScalingKeys;
			delete[] m_pBinAiScene->mAnimations[i].mChannels[j].mRotationKeys;
			delete[] m_pBinAiScene->mAnimations[i].mChannels[j].mPositionKeys;
		}
	}

	delete[] m_pBinAiScene->mAnimations;

	return S_OK;
}

const CBin_AIScene* CMyFile_Manager::Binary_InFile(const char* _strFilePath)
{
	m_pBinAiScene = CBin_AIScene::Create();
	// 파일을 열기 모드로 열기.
	ifstream fileStream(_strFilePath, ios::binary);
	if (fileStream.is_open()) {
		// 파일 내용을 읽기.

		Read_FBXNode(fileStream);
		Read_FBXMesh(fileStream);
		Read_FBXMaterial(fileStream);
		Read_FBXAnimation(fileStream);

		fileStream.close();
	}
	else {
		MSG_BOX("Fail BinFile Load");
	}

	return m_pBinAiScene;
}

HRESULT CMyFile_Manager::Read_FBXNode(ifstream& _inFile)
{
	size_t iStrLength = 0;

	_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mRootNode.mNumChildren), sizeof(unsigned int));

	_inFile.read(reinterpret_cast<char*>(&iStrLength), sizeof(size_t));
	char* newData = new char[iStrLength + 1];
	_inFile.read(newData, iStrLength * sizeof(char));
	newData[iStrLength] = '\0';
	m_pBinAiScene->mRootNode.mName.Set(newData);
	delete[] newData;

	_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mRootNode.mTransformation), sizeof(aiMatrix4x4));
	
	m_pBinAiScene->mRootNode.mChildren = new CBin_AIScene::DESC_NODE[m_pBinAiScene->mRootNode.mNumChildren];
	
	_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mRootNode.mChildren[0].mNumChildren), sizeof(unsigned int));
	
	_inFile.read(reinterpret_cast<char*>(&iStrLength), sizeof(size_t));
	newData = new char[iStrLength + 1];
	_inFile.read(newData, iStrLength * sizeof(char));
	newData[iStrLength] = '\0';
	m_pBinAiScene->mRootNode.mChildren[0].mName.Set(newData);
	delete[] newData;

	_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mRootNode.mChildren[0].mTransformation), sizeof(aiMatrix4x4));

	// 자식 노드 읽어들이기
	for (size_t i = 0; i < m_pBinAiScene->mRootNode.mNumChildren; ++i)
	{
		Read_FBXChildrenNode(_inFile, m_pBinAiScene->mRootNode.mChildren[i]);
	}

	return S_OK;
}

HRESULT CMyFile_Manager::Read_FBXChildrenNode(ifstream& _inFile, CBin_AIScene::DESC_NODE _node)
{
	CBin_AIScene::DESC_NODE tempNode;
	size_t iStrLength = 0;

	_inFile.read(reinterpret_cast<char*>(&_node.mNumChildren), sizeof(unsigned int));

	_inFile.read(reinterpret_cast<char*>(&iStrLength), sizeof(size_t));
	char* newData = new char[iStrLength + 1];
	_inFile.read(newData, iStrLength * sizeof(char));
	newData[iStrLength] = '\0';
	_node.mName.Set(newData);
	delete[] newData;

	_inFile.read(reinterpret_cast<char*>(&_node.mTransformation), sizeof(aiMatrix4x4));

	_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mRootNode.mChildren[0].mNumChildren), sizeof(unsigned int));

	_inFile.read(reinterpret_cast<char*>(&iStrLength), sizeof(size_t));
	newData = new char[iStrLength + 1];
	_inFile.read(newData, iStrLength * sizeof(char));
	newData[iStrLength] = '\0';
	m_pBinAiScene->mRootNode.mChildren[0].mName.Set(newData);
	delete[] newData;

	_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mRootNode.mChildren[0].mTransformation), sizeof(aiMatrix4x4));


	for (size_t i = 0; i < _node.mNumChildren; ++i)
	{
		Read_FBXChildrenNode(_inFile, _node.mChildren[i]);
	}

	return S_OK;
}

HRESULT CMyFile_Manager::Read_FBXMesh(ifstream& _inFile)
{
	return S_OK;
}

HRESULT CMyFile_Manager::Read_FBXMaterial(ifstream& _inFile)
{
	return S_OK;
}

HRESULT CMyFile_Manager::Read_FBXAnimation(ifstream& _inFile)
{
	return S_OK;
}

void CMyFile_Manager::Free()
{
	__super::Free();

	if (m_pBinAiScene != nullptr)
	{
		Safe_Release(m_pBinAiScene);
	}

	m_Importer.FreeScene();
}

