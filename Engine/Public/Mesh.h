#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer 
{
private:
	CMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const
	{
		return m_iMaterialIndex;
	}

	const _float3* Get_VtxPos() { return m_pPos; }
	vector<_ulong> Get_Indices() { return m_vecIndex; }

public:
	virtual HRESULT Initialize_ProtoType(const aiMesh* _pAIMesh, _fmatrix _matPivot);
	virtual HRESULT Initialize_Clone(void* _pArg) override;

private:
	_uint m_iMaterialIndex = 0;

	_float3* m_pPos = nullptr;
	vector<_ulong> m_vecIndex;

public:
	static CMesh* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const aiMesh* _pAIMesh, _fmatrix _matPivot);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

