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

public: /* ���� ����� �ε带 ����ϴ� �Լ��� */
	HRESULT Object_FileSave(ofstream& _outFile, _uint _iLevelIndex);
	HRESULT Object_FileLoad(ifstream& _inFile, _uint _iLevelIndex);

public: /* �� ���̳ʸ�ȭ�� ����ϴ� �Լ��� */
	HRESULT	Binary_OutFile(ofstream& _outFile, const CString& _strFilePath, CModel::MODEL_TYPE _eType);
	HRESULT Binary_NonAnimCopy(const char* _strFilePath, CModel::MODEL_TYPE _eType);
	
	class CBin_AIScene*	Binary_InFile(ifstream& _inFile, const char* _strFilePath);

private:
	Assimp::Importer	m_Importer; // FBX������ ��� �д´�.
	const aiScene*		m_pAIScene = nullptr;
	class CBin_AIScene* m_pBin_AIScene = nullptr;

public:
	virtual void Free() override;
};

END

