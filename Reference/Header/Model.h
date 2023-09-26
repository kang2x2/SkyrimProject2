#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_ProtoType(const char* _strModleFilePath);
	virtual HRESULT Initialize_Clone(void* pArg) override;

	HRESULT Render();

private:
	HRESULT Ready_Mesh();

private:
	// FBX������ ��� �д´�.
	Assimp::Importer m_Importer;
	// �а� ���� �����͵��� �����Ѵ�.
	const aiScene* m_pAIScene = nullptr;

	// numMeshes
	_uint			m_iNumMeshes = 0;
	// �� �ϳ��� �������� �޽ø� ���� �� �ִ�.(��, ���� ���)
	vector<class CMesh*> m_vMesh;

public:
	static CModel* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const char* _strModleFilePath);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;

};

END

