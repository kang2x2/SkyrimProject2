#pragma once

#include "Base.h"
#include "Model.h"

#include "Bin_AIScene.h"

BEGIN(Engine)

class CMyFile_Manager final : public CBase
{
	DECLARE_SINGLETON(CMyFile_Manager)

	/* �Ϲ����� ���� ����� */
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
	HRESULT StaticObject_FileSave(ofstream& _outFile, _uint _iLevelIndex);
	HRESULT DynamicObject_FileSave(ofstream& _outFile, _uint _iLevelIndex);
	HRESULT Object_FileLoad(ifstream& _inFile, _uint _iLevelIndex);

	HRESULT Cell_FileSave(ofstream& _outFile, class CNavigation* _pNavigation);
	HRESULT Cell_FileLoad(ifstream& _inFile, class CNavigation* _pNavigation);

public: /* ���̳ʸ� */
	HRESULT    Binary_OutFile(ofstream& _outFile, const char* _strFilePath, CModel::MODEL_TYPE _eType);

	HRESULT    Write_FBXNode(ofstream& _outFile, aiNode* _pNode, unsigned int _iParentIndex);
	HRESULT    Write_FBXMesh(ofstream& _outFile, CModel::MODEL_TYPE _eType);
	HRESULT    Write_FBXMaterial(ofstream& _outFile, const char* _strFilePath);
	HRESULT    Write_FBXAnimation(ofstream& _outFile);

	CBin_AIScene* Binary_InFile(const char* _strFilePath, CModel::MODEL_TYPE _eType);
	HRESULT    Read_FBXNode(ifstream& _inFile);
	HRESULT    Read_FBXMesh(ifstream& _inFile, CModel::MODEL_TYPE _eType);
	HRESULT    Read_FBXMaterial(ifstream& _inFile);
	HRESULT    Read_FBXAnimation(ifstream& _inFile);


private:
	Assimp::Importer	m_Importer; // FBX������ ��� �д´�.
	const aiScene* m_pAIScene = nullptr; // �а� ���� �����͵��� �����Ѵ�.

	CBin_AIScene* m_pBinAiScene = nullptr;
	vector<int>	  m_vecIndex;


public:
	virtual void Free() override;
};

END

