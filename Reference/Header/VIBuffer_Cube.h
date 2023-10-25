#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cube final : public CVIBuffer
{
private:
	CVIBuffer_Cube(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Cube(const CVIBuffer_Cube& rhs);
	virtual ~CVIBuffer_Cube() = default;

public:
	virtual HRESULT Initialize_ProtoType()		 override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	static CVIBuffer_Cube* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

