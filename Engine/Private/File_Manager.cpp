#include <fstream>

#include "File_Manager.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"
#include "Transform.h"

IMPLEMENT_SINGLETON(CFile_Manager)

CFile_Manager::CFile_Manager()
{

}

HRESULT CFile_Manager::Object_FileSave(ofstream& _outFile, _uint _iLevelIndex) const
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

HRESULT CFile_Manager::Object_FileLoad(std::ifstream& _inFile, _uint _iLevelIndex)
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

void CFile_Manager::Free()
{
	__super::Free();
}
