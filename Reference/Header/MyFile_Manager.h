#pragma once

#include "Base.h"
#include "Model.h"

BEGIN(Engine)

class CMyFile_Manager final : public CBase
{
	DECLARE_SINGLETON(CMyFile_Manager)

	typedef struct tagInOutFileDesc
	{
		wstring		m_strLayerTag;
		wstring		m_strProtoObjTag;
		wstring		m_strProtoModelComTag;
		_matrix     m_matWorld;
	}FILE_INOUTDESC;

private:
	CMyFile_Manager();
	virtual ~CMyFile_Manager() = default;

public: /* 파일 저장과 로드를 담당하는 함수들 */
	HRESULT Object_FileSave(ofstream& _outFile, _uint _iLevelIndex);
	HRESULT Object_FileLoad(ifstream& _inFile, _uint _iLevelIndex);

public: /* 모델 바이너리화를 담당하는 함수들 */
	HRESULT	Binary_OutFile(ofstream& _outFile, const CString& _strFilePath, CModel::MODEL_TYPE _eType);
	HRESULT Binary_NonAnimCopy(const char* _strFilePath, CModel::MODEL_TYPE _eType);
	
	class CBin_AIScene*	Binary_InFile(ifstream& _inFile, const char* _strFilePath);

private:
	Assimp::Importer	m_Importer; // FBX파일을 열어서 읽는다.
	const aiScene*		m_pAIScene = nullptr;
	class CBin_AIScene* m_pBin_AIScene = nullptr;

public:
	virtual void Free() override;
};

END

