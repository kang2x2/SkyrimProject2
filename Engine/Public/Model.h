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
	// FBX파일을 열어서 읽는다.
	Assimp::Importer m_Importer;
	// 읽고 얻은 데이터들을 보관한다.
	const aiScene* m_pAIScene = nullptr;

	// numMeshes
	_uint			m_iNumMeshes = 0;
	// 모델 하나가 여러개의 메시를 가질 수 있다.(옷, 무기 등등)
	vector<class CMesh*> m_vMesh;

public:
	static CModel* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const char* _strModleFilePath);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;

};

END

