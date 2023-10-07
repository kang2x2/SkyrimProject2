#pragma once

#include "Base.h"

BEGIN(Engine)

class CFile_Manager final : public CBase
{
	DECLARE_SINGLETON(CFile_Manager)
private:
	typedef struct tagInOutFileDesc
	{
		wstring		m_strLayerTag = TEXT("");
		wstring		m_strProtoObjTag = TEXT("");
		wstring		m_strProtoModelComTag = TEXT("");
		_float3		m_vScale;
		_float3     m_vRotation;
		_float4     m_vTranslation;
	}FILE_INOUTDESC;

private:
	CFile_Manager();
	virtual ~CFile_Manager() = default;

public: /* ���� ����� �ε带 ����ϴ� �Լ��� */
	virtual HRESULT Object_FileSave(ofstream& _outFile, _uint _iLevelIndex) const;
	virtual HRESULT Object_FileLoad(std::ifstream& _inFile, _uint _iLevelIndex);

public:
	virtual void Free() override;
};

END

