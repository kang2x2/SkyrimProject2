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
		MSG_BOX("Fail Binary Write : RootNode");
		return E_FAIL;
	}
	if (FAILED(Write_FBXChildrenNode(_outFile, m_pAIScene->mRootNode, -1)))
	{
		MSG_BOX("Fail Binary Write : ChildrenNode");
		return E_FAIL;
	}

	unsigned int Escapevalue = static_cast<unsigned int>(9999);
	_outFile.write(reinterpret_cast<const char*>(&Escapevalue), sizeof(unsigned int));

	if (FAILED(Write_FBXMesh(_outFile, _eType)))
	{
		MSG_BOX("Fail Binary Write : Mesh");
		return E_FAIL;
	}
	if (FAILED(Write_FBXMaterial(_outFile, _strFilePath)))
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
	size_t iStrLength = 0;

	/* Root Node */
	unsigned int mParentIndex = -1;

	_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mRootNode->mNumChildren), sizeof(unsigned int));
	
	iStrLength = m_pAIScene->mRootNode->mName.length;
	_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
	_outFile.write(reinterpret_cast<const char*>(m_pAIScene->mRootNode->mName.C_Str()), iStrLength * sizeof(char));

	_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mRootNode->mTransformation), sizeof(aiMatrix4x4));
	_outFile.write(reinterpret_cast<const char*>(&mParentIndex), sizeof(unsigned int));

	return S_OK;
}

HRESULT CMyFile_Manager::Write_FBXChildrenNode(ofstream& _outFile, aiNode* _pNode, unsigned int _iParentIndex)
{
	if (_pNode->mNumChildren > 0)
	{
		_outFile.write(reinterpret_cast<const char*>(&_pNode->mNumChildren), sizeof(unsigned int));
		
		size_t iStrLength = _pNode->mName.length;
		_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
		_outFile.write(reinterpret_cast<const char*>(_pNode->mName.data), iStrLength * sizeof(char));

		_outFile.write(reinterpret_cast<const char*>(&_pNode->mTransformation), sizeof(aiMatrix4x4));
		_outFile.write(reinterpret_cast<const char*>(&_iParentIndex + 1), sizeof(unsigned int));

		for (size_t i = 0; i < _pNode->mNumChildren; ++i)
		{
			Write_FBXChildrenNode(_outFile, _pNode->mChildren[i], _iParentIndex + 1);
		}
	}
	return S_OK;
}

HRESULT CMyFile_Manager::Write_FBXMesh(ofstream& _outFile, CModel::MODEL_TYPE _eType)
{
	_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mNumMeshes), sizeof(unsigned int));

	size_t iStrLength = 0;

	/* 전체 메시 */
	for (size_t i = 0; i < m_pAIScene->mNumMeshes; ++i)
	{
		iStrLength = m_pAIScene->mMeshes[i]->mName.length;
		_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
		_outFile.write(reinterpret_cast<const char*>(m_pAIScene->mMeshes[i]->mName.data), iStrLength * sizeof(char));
	}

	/* 재질 */
	for (size_t i = 0; i < m_pAIScene->mNumMeshes; ++i)
	{
		_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mMeshes[i]->mMaterialIndex), sizeof(unsigned int));
	}

	/* 메시 면 */
	for (size_t i = 0; i < m_pAIScene->mNumMeshes; ++i)
	{
		_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mMeshes[i]->mNumFaces), sizeof(unsigned int));

		for (size_t j = 0; j < m_pAIScene->mMeshes[i]->mNumFaces; ++j)
		{
			_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mMeshes[i]->mNumFaces), sizeof(unsigned int) * 3);
		}
	}

	/* 버텍스 */
	for (size_t i = 0; i < m_pAIScene->mNumMeshes; ++i)
	{
		_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mMeshes[i]->mNumVertices), sizeof(unsigned int));
		
		for (size_t j = 0; j < m_pAIScene->mMeshes[i]->mNumVertices; ++j)
		{
			_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mMeshes[i]->mVertices[j]), sizeof(_float3));
			_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mMeshes[i]->mNormals[j]), sizeof(_float3));
			_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mMeshes[i]->mTextureCoords[0][j]), sizeof(_float2));
			_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mMeshes[i]->mTangents[j]), sizeof(_float3));
		}
	}

	/* 메시 본 */
	if (_eType == CModel::TYPE_ANIM)
	{
		for (size_t i = 0; i < m_pAIScene->mNumMeshes; ++i)
		{
			if (m_pAIScene->mMeshes[i]->mBones == nullptr)
				continue;

			_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mMeshes[i]->mNumBones), sizeof(unsigned int));

			for (size_t j = 0; j < m_pAIScene->mMeshes[i]->mNumBones; ++j)
			{
				if (m_pAIScene->mMeshes[i]->mBones[j] == nullptr)
					continue;

				_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mMeshes[i]->mBones[j]->mOffsetMatrix), sizeof(aiMatrix4x4));

				iStrLength = m_pAIScene->mMeshes[i]->mBones[j]->mName.length;
				_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
				_outFile.write(reinterpret_cast<const char*>(m_pAIScene->mMeshes[i]->mBones[j]->mName.data), iStrLength * sizeof(char));

				_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mMeshes[i]->mBones[j]->mNumWeights), sizeof(unsigned int));

				/* 메시 본 웨이트 */
				for (size_t k = 0; k < m_pAIScene->mMeshes[i]->mBones[j]->mNumWeights; ++k)
				{
					_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mMeshes[i]->mBones[j]->mWeights[k].mVertexId), sizeof(unsigned int));
					_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mMeshes[i]->mBones[j]->mWeights[k].mWeight), sizeof(_float));
				}
			}
		}
	}

	return S_OK;
}

HRESULT CMyFile_Manager::Write_FBXMaterial(ofstream& _outFile, const char* _strFilePath)
{
	_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mNumMaterials), sizeof(unsigned int));

	for (size_t i = 0; i < m_pAIScene->mNumMaterials; ++i)
	{
		aiMaterial* pMaterials = m_pAIScene->mMaterials[i];

		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString strTexturePath;

			// 해당 재질 정보가 없으면 다음으로
			aiReturn eReturn = pMaterials->GetTexture(aiTextureType(j), 0, &strTexturePath);
			int iReturnNum = 0;
			if (eReturn == aiReturn_SUCCESS)
				iReturnNum = 1;

			_outFile.write(reinterpret_cast<const char*>(&iReturnNum), sizeof(_int));
			
			if (FAILED(eReturn))
				continue;

			// 드라이브, 경로 추출
			char			szDrive[MAX_PATH] = "";
			char			szDirectory[MAX_PATH] = "";
			_splitpath_s(_strFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

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

			size_t iStrLength = strlen(szTmp); // strlen을 사용하여 문자열 길이 가져오기
			_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
			_outFile.write(szTmp, iStrLength * sizeof(char)); // 문자열 데이터를 저장
		}
	}



	return S_OK;
}

HRESULT CMyFile_Manager::Write_FBXAnimation(ofstream& _outFile)
{
	_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mNumAnimations), sizeof(unsigned int));

	size_t iStrLength = 0;
	for (size_t i = 0; i < m_pAIScene->mNumAnimations; ++i)
	{
		iStrLength = m_pAIScene->mAnimations[i]->mName.length;
		_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
		_outFile.write(reinterpret_cast<const char*>(m_pAIScene->mAnimations[i]->mName.data), iStrLength * sizeof(char));
		
		m_pAIScene->mAnimations[i]->mDuration = (_float)m_pAIScene->mAnimations[i]->mDuration;
		m_pAIScene->mAnimations[i]->mTicksPerSecond = (_float)m_pAIScene->mAnimations[i]->mTicksPerSecond;
		
		_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mAnimations[i]->mDuration), sizeof(_float));
		_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mAnimations[i]->mTicksPerSecond), sizeof(_float));
		_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mAnimations[i]->mNumChannels), sizeof(unsigned int));

		for (size_t j = 0; j < m_pAIScene->mAnimations[i]->mNumChannels; ++j)
		{
			iStrLength = m_pAIScene->mAnimations[i]->mChannels[j]->mNodeName.length;
			_outFile.write(reinterpret_cast<const char*>(&iStrLength), sizeof(size_t));
			_outFile.write(reinterpret_cast<const char*>(m_pAIScene->mAnimations[i]->mChannels[j]->mNodeName.data), iStrLength * sizeof(char));
			
			_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mAnimations[i]->mChannels[j]->mNumScalingKeys), sizeof(unsigned int));
			_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mAnimations[i]->mChannels[j]->mNumRotationKeys), sizeof(unsigned int));
			_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mAnimations[i]->mChannels[j]->mNumPositionKeys), sizeof(unsigned int));
		
			for (size_t k = 0; k < m_pAIScene->mAnimations[i]->mChannels[j]->mNumScalingKeys; ++k)
			{
				m_pAIScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mTime = (_float)m_pAIScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mTime;
			
				_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mTime), sizeof(_float));
				_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue), sizeof(_float3));
			}
			for (size_t k = 0; k < m_pAIScene->mAnimations[i]->mChannels[j]->mNumRotationKeys; ++k)
			{
				m_pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mTime = (_float)m_pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mTime;
			
				_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mTime), sizeof(_float));
				_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue), sizeof(_float4));
			}
			for (size_t k = 0; k < m_pAIScene->mAnimations[i]->mChannels[j]->mNumPositionKeys; ++k)
			{
				m_pAIScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mTime = (_float)m_pAIScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mTime;
			
				_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mTime), sizeof(_float));
				_outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue), sizeof(_float3));
			}
		}
	}

	return S_OK;
}

const CBin_AIScene* CMyFile_Manager::Binary_InFile(const char* _strFilePath, CModel::MODEL_TYPE _eType)
{
	m_pBinAiScene = CBin_AIScene::Create();

	// 파일을 열기 모드로 열기.
	ifstream fileStream(_strFilePath, ios::binary);
	if (fileStream.is_open()) {
		// 파일 내용을 읽기.
		if (FAILED(Read_FBXNode(fileStream)))
		{
			MSG_BOX("Fail Read : RootNode");
		}
		//if (FAILED(Read_FBXChildrenNode(fileStream)))
		//{
		//	MSG_BOX("Fail Read : ChildrenNode");
		//}
		if (FAILED(Read_FBXMesh(fileStream, _eType)))
		{
			MSG_BOX("Fail Read : Mesh");
		}
		if (FAILED(Read_FBXMaterial(fileStream)))
		{
			MSG_BOX("Fail Read : Material");
		}
		if (FAILED(Read_FBXAnimation(fileStream)))
		{
			MSG_BOX("Fail Read : Animation");
		}

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
	unsigned int iEscapeValue = 0;


	while (true) {
		
		streampos startPos = _inFile.tellg();
		_inFile.read(reinterpret_cast<char*>(&iEscapeValue), sizeof(unsigned int));
		if (iEscapeValue == 9999) 
			break;
		_inFile.seekg(startPos);

		CBin_AIScene::DESC_NODE tempRootNode;

		_inFile.read(reinterpret_cast<char*>(&tempRootNode.mNumChildren), sizeof(unsigned int));

		_inFile.read(reinterpret_cast<char*>(&iStrLength), sizeof(size_t));
		char* newData = new char[iStrLength + 1];
		_inFile.read(newData, iStrLength * sizeof(char));
		newData[iStrLength] = '\0';
		tempRootNode.mName.Set(newData);
		delete[] newData;

		_inFile.read(reinterpret_cast<char*>(&tempRootNode.mTransformation), sizeof(aiMatrix4x4));
		_inFile.read(reinterpret_cast<char*>(&tempRootNode.mParentIndex), sizeof(unsigned int));

		m_pBinAiScene->mVecNode.push_back(tempRootNode);
		
	}
	return S_OK;
}

//HRESULT CMyFile_Manager::Read_FBXChildrenNode(ifstream& _inFile)
//{
//	CBin_AIScene::DESC_NODE tempNode;
//	size_t iStrLength = 0;
//
//	_inFile.read(reinterpret_cast<char*>(&tempNode.mNumChildren), sizeof(unsigned int));
//
//	_inFile.read(reinterpret_cast<char*>(&iStrLength), sizeof(size_t));
//	char* newData = new char[iStrLength + 1];
//	_inFile.read(newData, iStrLength * sizeof(char));
//	newData[iStrLength] = '\0';
//	tempNode.mName.Set(newData);
//	delete[] newData;
//
//	_inFile.read(reinterpret_cast<char*>(&tempNode.mTransformation), sizeof(aiMatrix4x4));
//	_inFile.read(reinterpret_cast<char*>(&tempNode.mParentIndex), sizeof(unsigned int));
//
//	m_pBinAiScene->mVecNode.push_back(tempNode);
//
//	for (size_t i = 0; i < _node.mNumChildren; ++i)
//	{
//		Read_FBXChildrenNode(_inFile);
//	}
//
//	return S_OK;
//}

HRESULT CMyFile_Manager::Read_FBXMesh(ifstream& _inFile, CModel::MODEL_TYPE _eType)
{
	size_t iStrLength = 0;

	_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mNumMeshes), sizeof(unsigned int));

	m_pBinAiScene->mMeshs = new CBin_AIScene::DESC_MESH[m_pBinAiScene->mNumMeshes];

	/* 전체 메시 */
	for (size_t i = 0; i < m_pBinAiScene->mNumMeshes; ++i)
	{
		_inFile.read(reinterpret_cast<char*>(&iStrLength), sizeof(size_t));
		char* newData = new char[iStrLength + 1];
		_inFile.read(newData, iStrLength * sizeof(char));
		newData[iStrLength] = '\0';
		m_pBinAiScene->mMeshs[i].mName.Set(newData);
		delete[] newData;
	}

	/* 재질 */
	for (size_t i = 0; i < m_pBinAiScene->mNumMeshes; ++i)
	{
		_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mMeshs[i].mMaterialIndex), sizeof(unsigned int));
	}

	/* 메시 면 */
	for (size_t i = 0; i < m_pBinAiScene->mNumMeshes; ++i)
	{
		_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mMeshs[i].mNumFaces), sizeof(unsigned int));

		m_pBinAiScene->mMeshs[i].mFaces = new CBin_AIScene::DESC_MESHFACE[m_pBinAiScene->mMeshs[i].mNumFaces];

		for (size_t j = 0; j < m_pBinAiScene->mMeshs[i].mNumFaces; ++j)
		{
			_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mMeshs[i].mFaces[j].mIndices), sizeof(unsigned int) * 3);
		}

		//delete[] m_pBinAiScene->mMeshs[i].mFaces;
	}

	/* 버텍스 */
	for (size_t i = 0; i < m_pBinAiScene->mNumMeshes; ++i)
	{
		_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mMeshs[i].mNumVertices), sizeof(unsigned int));

		m_pBinAiScene->mMeshs[i].mVertices = new aiVector3D[m_pBinAiScene->mMeshs[i].mNumVertices];
		m_pBinAiScene->mMeshs[i].mNormals = new aiVector3D[m_pBinAiScene->mMeshs[i].mNumVertices];
		m_pBinAiScene->mMeshs[i].mTextureCoords[0] = new aiVector3D[m_pBinAiScene->mMeshs[i].mNumVertices];
		m_pBinAiScene->mMeshs[i].mTangents = new aiVector3D[m_pBinAiScene->mMeshs[i].mNumVertices];

		for (size_t j = 0; j < m_pBinAiScene->mMeshs[i].mNumVertices; ++j)
		{
			_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mMeshs[i].mVertices[j]), sizeof(_float3));
			_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mMeshs[i].mNormals[j]), sizeof(_float3));
			_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mMeshs[i].mTextureCoords[0][j]), sizeof(_float2));
			_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mMeshs[i].mTangents[j]), sizeof(_float3));
		}

		//delete[] m_pBinAiScene->mMeshs[i].mVertices;
		//delete[] m_pBinAiScene->mMeshs[i].mNormals;
		//delete[] m_pBinAiScene->mMeshs[i].mTextureCoords[0];
		//delete[] m_pBinAiScene->mMeshs[i].mTangents;
	}

	/* 메시 본 */
	if (_eType == CModel::TYPE_ANIM)
	{
		for (size_t i = 0; i < m_pBinAiScene->mNumMeshes; ++i)
		{
			//if (m_pBinAiScene->mMeshs[i]->mBones == nullptr)
			//	continue;

			_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mMeshs[i].mNumBones), sizeof(unsigned int));

			m_pBinAiScene->mMeshs[i].mBones = new CBin_AIScene::DESC_MESHBONE[m_pBinAiScene->mMeshs[i].mNumBones];
			for (size_t j = 0; j < m_pBinAiScene->mMeshs[i].mNumBones; ++j)
			{
				//if (m_pAIScene->mMeshes[i]->mBones[j] == nullptr)
				//	continue;

				_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mMeshs[i].mBones[j].mOffsetMatrix), sizeof(aiMatrix4x4));

				_inFile.read(reinterpret_cast<char*>(&iStrLength), sizeof(size_t));
				char* newData = new char[iStrLength + 1];
				_inFile.read(newData, iStrLength * sizeof(char));
				newData[iStrLength] = '\0';
				m_pBinAiScene->mMeshs[i].mBones[j].mName.Set(newData);
				delete[] newData;

				_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mMeshs[i].mBones[j].mNumWeights), sizeof(unsigned int));

				/* 메시 본 웨이트 */
				m_pBinAiScene->mMeshs[i].mBones[j].mWeights = new CBin_AIScene::DESC_MESHBONEWEIGHT[m_pBinAiScene->mMeshs[i].mBones[j].mNumWeights];
				for (size_t k = 0; k < m_pBinAiScene->mMeshs[i].mBones[j].mNumWeights; ++k)
				{
					_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mMeshs[i].mBones[j].mWeights[k].mVertexId), sizeof(unsigned int));
					_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mMeshs[i].mBones[j].mWeights[k].mWeight), sizeof(_float));
				}
				//delete[] m_pBinAiScene->mMeshs[i].mBones[j].mWeights;
			}
			//delete[] m_pBinAiScene->mMeshs[i].mBones;
		}
	}

	//delete[] m_pBinAiScene->mMeshs;

	return S_OK;
}

HRESULT CMyFile_Manager::Read_FBXMaterial(ifstream& _inFile)
{
	_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mNumMaterials), sizeof(unsigned int));
	
	for (size_t i = 0; i < m_pBinAiScene->mNumMaterials; ++i)
	{
		CBin_AIScene::DESC_MATERIAL tempMaterial;

		m_pBinAiScene->mMeshs = new CBin_AIScene::DESC_MESH[m_pBinAiScene->mNumMeshes];

		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			_inFile.read(reinterpret_cast<char*>(&tempMaterial.mBIsReturn[j]), sizeof(_int));

			if (!tempMaterial.mBIsReturn[j])
				continue;

			size_t iStrLength = 0;
			_inFile.read(reinterpret_cast<char*>(&iStrLength), sizeof(size_t));

			char* readBuffer = new char[iStrLength + 1];
			_inFile.read(readBuffer, iStrLength * sizeof(char));
			readBuffer[iStrLength] = '\0';

			const char* constStr = readBuffer;

			// delete[] readBuffer;

			tempMaterial.mStrTextureFilePath[j] = constStr;
			m_pBinAiScene->mMaterials.push_back(tempMaterial);
		}
	}


	return S_OK;
}

HRESULT CMyFile_Manager::Read_FBXAnimation(ifstream& _inFile)
{
	size_t iStrLength = 0;

	_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->m_iNumAnimation), sizeof(unsigned int));

	m_pBinAiScene->mAnimations = new CBin_AIScene::DESC_ANIMATION[m_pBinAiScene->m_iNumAnimation];

	for (size_t i = 0; i < m_pBinAiScene->m_iNumAnimation; ++i)
	{
		_inFile.read(reinterpret_cast<char*>(&iStrLength), sizeof(size_t));
		char* newData = new char[iStrLength + 1];
		_inFile.read(newData, iStrLength * sizeof(char));
		newData[iStrLength] = '\0';
		m_pBinAiScene->mAnimations[i].mName.Set(newData);
		delete[] newData;

		_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mAnimations[i].mDuration), sizeof(_float));
		_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mAnimations[i].mTicksPerSecond), sizeof(_float));
		_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mAnimations[i].mNumChannels), sizeof(unsigned int));

		m_pBinAiScene->mAnimations[i].mChannels = new CBin_AIScene::DESC_ANIMATIONCHANNEL[m_pBinAiScene->mAnimations[i].mNumChannels];

		for (size_t j = 0; j < m_pBinAiScene->mAnimations[i].mNumChannels; ++j)
		{
			_inFile.read(reinterpret_cast<char*>(&iStrLength), sizeof(size_t));
			char* newData = new char[iStrLength + 1];
			_inFile.read(newData, iStrLength * sizeof(char));
			newData[iStrLength] = '\0';
			m_pBinAiScene->mAnimations[i].mChannels[j].mNodeName.Set(newData);
			delete[] newData;

			_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mAnimations[i].mChannels[j].mNumScalingKeys), sizeof(unsigned int));
			_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mAnimations[i].mChannels[j].mNumRotationKeys), sizeof(unsigned int));
			_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mAnimations[i].mChannels[j].mNumPositionKeys), sizeof(unsigned int));

			m_pBinAiScene->mAnimations[i].mChannels[j].mScalingKeys = new CBin_AIScene::DESC_CHANNELSCALEKEY[m_pBinAiScene->mAnimations[i].mChannels[j].mNumScalingKeys];
			m_pBinAiScene->mAnimations[i].mChannels[j].mRotationKeys = new CBin_AIScene::DESC_CHANNELROTKEY[m_pBinAiScene->mAnimations[i].mChannels[j].mNumRotationKeys];
			m_pBinAiScene->mAnimations[i].mChannels[j].mPositionKeys = new CBin_AIScene::DESC_CHANNELPOSKEY[m_pBinAiScene->mAnimations[i].mChannels[j].mNumPositionKeys];

			for (size_t k = 0; k < m_pBinAiScene->mAnimations[i].mChannels[j].mNumScalingKeys; ++k)
			{
				_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mAnimations[i].mChannels[j].mScalingKeys[k].mTime), sizeof(_float));
				_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mAnimations[i].mChannels[j].mScalingKeys[k].mValue), sizeof(_float3));
			}
			for (size_t k = 0; k < m_pBinAiScene->mAnimations[i].mChannels[j].mNumRotationKeys; ++k)
			{
				_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mAnimations[i].mChannels[j].mRotationKeys[k].mTime), sizeof(_float));
				_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mAnimations[i].mChannels[j].mRotationKeys[k].mValue), sizeof(_float4));
			}
			for (size_t k = 0; k < m_pBinAiScene->mAnimations[i].mChannels[j].mNumPositionKeys; ++k)
			{
				_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mAnimations[i].mChannels[j].mPositionKeys[k].mTime), sizeof(_float));
				_inFile.read(reinterpret_cast<char*>(&m_pBinAiScene->mAnimations[i].mChannels[j].mPositionKeys[k].mValue), sizeof(_float3));
			}

			//delete[] m_pBinAiScene->mAnimations[i].mChannels[j].mScalingKeys;
			//delete[] m_pBinAiScene->mAnimations[i].mChannels[j].mRotationKeys;
			//delete[] m_pBinAiScene->mAnimations[i].mChannels[j].mPositionKeys;
		}
		//delete[] m_pBinAiScene->mAnimations[i].mChannels;
	}

	//delete[] m_pBinAiScene->mAnimations;


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

