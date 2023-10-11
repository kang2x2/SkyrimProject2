#pragma once

#include "Base.h"
#include "Model.h"

BEGIN(Engine)

class CMyFile_Manager final : public CBase
{
	DECLARE_SINGLETON(CMyFile_Manager)

	/* 일반적인 파일 입출력 */
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

public: /* 바이너리 */
	HRESULT    Binary_OutFile(ofstream& _outFile, const char* _strFilePath, CModel::MODEL_TYPE _eType);

public:
	virtual void Free() override;
};

END

