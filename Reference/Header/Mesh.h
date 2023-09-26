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
	virtual HRESULT Initialize_ProtoType(const aiMesh* _pAIMesh);
	virtual HRESULT Initialize_Clone(void* _pArg) override;

public:
	static CMesh* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const aiMesh* _pAIMesh);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

